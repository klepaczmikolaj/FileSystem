#include "include/FileSystem.h"

using std::cout;
using std::endl;
using boost::filesystem::file_size;

FileSystem::FileSystem(){

}

FileSystem::~FileSystem(){
    if(fileFS.is_open())
        fileFS.close();
}

void FileSystem::createFileSystem(int size, std::string name){
    // file allocation table size depends on predefined size of a File System
    // roughly 20% of total storage is preserved for metadata nodes
    // additional block for system info is at the beginning
    systemInfo.sizeKB = size;
    strcpy(systemInfo.name, name.c_str());
    systemInfo.totalBlocks = size * KILOBYTE / BLOCK_SIZE;
    systemInfo.fatSize = ceil(systemInfo.totalBlocks / 10.0) * 2 + 1;
    systemInfo.usedBlocks = systemInfo.fatSize;
    systemInfo.numberOfFiles = 0;

    fileFS.open(name.c_str(), std::ios::binary | std::ios::out);
    if(!fileFS.is_open()){
        cout << "Error opening FS file" << endl;
        return;
    }
    fillSysWithZeros();
    writeSystemInfo();
}

void FileSystem::openFileSystem(std::string name){
    fileFS.open(name.c_str(), std::ios::binary | std::ios::in | std::ios::out);
    if(!fileFS.is_open()){
        cout << "Error opening FS file" << endl;
        return;
    }
    readSystemInfo(name);
    readMetadata();
}

void FileSystem::closeFileSystem(){
    writeSystemInfo();
    writeMetadata();
}

void FileSystem::loadFile(std::string name){
    if(doesFileExistInFS(name)){
        cout << "File already exists in file system and will not be loaded" << endl;
        return;
    }
    int nodeID = appendFileInfoToMetadata(name);
    if(nodeID < 0)
        return;
    copyFileDataToFS(nodeID);
    sortMetadata();
    systemInfo.numberOfFiles++;
}

void FileSystem::downloadFile(std::string fileName){
    for(unsigned i = 0; i < inodes.size(); i++)
        if(inodes[i].name == fileName){
            writeFileToCurrentDir(i);
            return;
        }
    cout << "File " << fileName << " not found in the directory" << endl;
}

void FileSystem::displayCatalog(){
    cout << "Files in a catalog: " << endl;
    cout << "Name \t Size(bytes)" << endl;
    if(inodes.empty())
        cout << "[EMPTY]" << endl;
    for(FileNode node : inodes)
        cout << node.name << "\t" << node.sizeInBytes << endl;
}

void FileSystem::deleteFile(std::string fileName){
    for(unsigned i = 0; i < inodes.size(); i++)
        if(inodes[i].name == fileName){
            inodes.erase(inodes.begin() + i);
            return;
        }
    cout << "File " << fileName << " not found in file system" << endl;
}

void FileSystem::displayMemoryMap(){
    cout << std::setw(10) << "address" << std::setw(20) << "type";
    cout << std::setw(10) << "size" << std::setw(10) << "state" << endl;

    cout << std::setw(10) << 1 << std::setw(20) << "SYS Info";
    cout << std::setw(10) << BLOCK_SIZE << std::setw(10) << "used" << endl;

    for(int i = 2; i < systemInfo.fatSize + 1; i++){
        cout << std::setw(10) << i << std::setw(20) << "File metadata";
        cout << std::setw(10) << BLOCK_SIZE << std::setw(10) << "used" << endl;
    }
    for(int i = systemInfo.fatSize + 1; i < systemInfo.totalBlocks + 1; i++){
        std::string message1, message2, filename;
        if(isBlockUsed(i)){
            message1 = getFileNameFromMemID(i);
            message2 = "used";
        }
        else{
            message1 = "File data";
            message2 = "empty";
        }
        cout << std::setw(10) << i << std::setw(20) << message1;
        cout << std::setw(10) << BLOCK_SIZE << std::setw(10) << message2 << endl;
    }
}

void FileSystem::FileSystem::displaySystemInfo(){
    cout << "file System Type: " << systemInfo.fileSystemType << endl;
    cout << "name: " << systemInfo.name << endl;
    cout << "size in kB " << systemInfo.sizeKB << endl;
    cout << "total blocks number " << systemInfo.totalBlocks << endl;
    cout << "used blocks " << systemInfo.usedBlocks << endl;
}

void FileSystem::readSystemInfo(std::string name){
    blockBuffer buf;
    fileFS.read(buf, sizeof(systemInfo));
    memcpy(&systemInfo, buf, sizeof(SystemInfo));
}

void FileSystem::writeSystemInfo(){
    blockBuffer buf;
    memcpy(buf, &systemInfo, sizeof(SystemInfo));
    fileFS.seekp(std::ios_base::beg);
    fileFS.write(buf, sizeof(systemInfo));
}

void FileSystem::readMetadata(){
    int fileNumber = systemInfo.numberOfFiles;
    for(int i = 0; i < fileNumber; i++){
        FileNode node;
        int nodeSize = sizeof(FileNode);
        char nodeBuffer[nodeSize];
        fileFS.seekp(BLOCK_SIZE + i * nodeSize);
        fileFS.read(nodeBuffer, nodeSize);
        memcpy(&node, nodeBuffer, nodeSize);
        inodes.push_back(node);
    }
}

void FileSystem::writeMetadata(){
    int bufferNumber = 0;
    for(FileNode node : inodes){
        int nodeSize = sizeof(FileNode);
        char nodeBuffer[nodeSize];
        memcpy(nodeBuffer, &node, nodeSize);
        fileFS.seekp(BLOCK_SIZE + bufferNumber * nodeSize);
        fileFS.write(nodeBuffer, nodeSize);
        bufferNumber++;
    }
}

int FileSystem::getMemoryPosition(int blocksNumber){
    if(blocksNumber > systemInfo.totalBlocks - systemInfo.usedBlocks){
        return -1;
    }
    // empty or check if begining is available
    else if(inodes.empty() || (inodes.size() > 0 && inodes.front().startBlock - systemInfo.fatSize > blocksNumber)){
        return systemInfo.fatSize + 1;
    }
    // check place after the only inode
    else if(inodes.size() == 1 && systemInfo.totalBlocks - getNodeEndBlock(inodes.front()) > blocksNumber){
        return getNodeEndBlock(inodes.front()) + 1;
    }
    else{
        // check places between blocks
        for(unsigned int i = 0; i < inodes.size() - 1; i++){
            if(inodes[i + 1].startBlock - getNodeEndBlock(inodes[i]) > blocksNumber)
                return getNodeEndBlock(inodes[i]) + 1;
        }
        // check place in the end
        if(systemInfo.totalBlocks - getNodeEndBlock(inodes.back()) > blocksNumber)
            return getNodeEndBlock(inodes.back()) + 1;
    }
    return 0;
}

int FileSystem::appendFileInfoToMetadata(std::string name){
    FileNode newNode;
    strcpy(newNode.name, name.c_str());
    newNode.sizeInBytes = file_size(name);
    newNode.blocksNumber = ceil(newNode.sizeInBytes / (float)BLOCK_SIZE);
    newNode.startBlock = getMemoryPosition(newNode.blocksNumber);
    systemInfo.usedBlocks += newNode.blocksNumber;
    if(newNode.startBlock == -1){
        cout << "Not enough memory to store the file" << endl;
        return -1;
    }
    else if(newNode.startBlock == 0){
        //if(defragmentMemory() == -1){
        //    cout << "Defragmentation failed, unable to load file due to memory fragmentation" << endl;
        //    return -1;
        //}
        defragmentMemory();
        newNode.startBlock = getMemoryPosition(newNode.blocksNumber);
    }
    inodes.push_back(newNode);
    return inodes.size() - 1;
}

int FileSystem::defragmentMemory(){
    blockBuffer buf;
    int nextBlock = systemInfo.fatSize + 1;
    int i = 0;
    for(FileNode node : inodes){
        fileFS.seekp(BLOCK_SIZE * node.startBlock);
        fileFS.read(buf, sizeof(buf));

        fileFS.seekp(BLOCK_SIZE * nextBlock);
        fileFS.write(buf, sizeof(buf));
        
        node.startBlock = nextBlock;
        nextBlock = getNodeEndBlock(node) + 1;
        i++;
    }
    return 0;
    //return -1;
}

void FileSystem::copyFileDataToFS(int nodeID){
    blockBuffer buf;
    FileNode node = inodes[nodeID];
    std::ifstream infile(node.name);
    if(!infile.is_open()){
        cout << "Error opening " << node.name << " file" << endl;
        return;
    }
    for(int i = 0; i < node.blocksNumber; i++){
        infile.read(buf, sizeof(buf));
        fileFS.seekp((node.startBlock + i) * BLOCK_SIZE);
        fileFS.write(buf, sizeof(buf));
    }
    infile.close();
}

void FileSystem::writeFileToCurrentDir(int nodeID){
    blockBuffer buf;
    FileNode node = inodes[nodeID];
    if(doesFileExistInDIR(node.name)){
        cout << "File " << node.name << " already exists in a directory ";
        cout << "and will be replaced" << endl;
    }
    std::ofstream outfile(node.name);
    if(!outfile.is_open()){
        cout << "Error opening " << node.name << " file" << endl;
        return;
    }
    for(int i = 0; i < node.blocksNumber; i++){
        fileFS.seekp((node.startBlock + i) * BLOCK_SIZE);
        int size = sizeof(buf);
        if(i == node.blocksNumber - 1){
            size = node.sizeInBytes - i * sizeof(buf);
        }
        fileFS.read(buf, size);
        outfile.write(buf, size);
    }
    outfile.close();
    
}

void FileSystem::sortMetadata(){
    if(!inodes.empty())
        sort(inodes.begin(), inodes.end(), FileSystem::compareNodes);
}

bool FileSystem::compareNodes(FileNode node1, FileNode node2){
    return node1.startBlock < node2.startBlock;
}

void FileSystem::fillSysWithZeros(){
    blockBuffer buf;
    for(unsigned int i = 0; i < BLOCK_SIZE; i++)
        buf[i] = '\0';

    fileFS.seekp(std::ios_base::beg);
    for(int i = 0; i < systemInfo.totalBlocks; i++){
        fileFS.write(buf, BLOCK_SIZE);
    }
}

int FileSystem::getNodeEndBlock(FileNode node){
    return node.startBlock + node.blocksNumber - 1;
}

bool FileSystem::doesFileExistInDIR(std::string fileName){
    std::ifstream file(fileName.c_str());
    return file.good();
}

bool FileSystem::doesFileExistInFS(std::string fileName){
    for(auto node : inodes){
        if(node.name == fileName)
            return true;
    }
    return false;
}

bool FileSystem::isBlockUsed(int memID){
    for(auto node : inodes){
        if(memID >= node.startBlock && memID <= getNodeEndBlock(node))
            return true;
    }
    return false;
}

std::string FileSystem::getFileNameFromMemID(int memID){
    for(auto node : inodes){
        if(memID >= node.startBlock && memID <= getNodeEndBlock(node))
            return node.name;
    }
    return "none";
}