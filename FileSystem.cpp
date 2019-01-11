#include "FileSystem.h"

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
    // roughly 10% of total storage is preserved for metadata node
    // additional block for system info is at the beginning
    systemInfo.sizeKB = size;
    strcpy(systemInfo.name, name.c_str());
    systemInfo.totalBlocks = size * KILOBYTE / BLOCK_SIZE;
    systemInfo.fatSize = round(systemInfo.totalBlocks / 10.0) + 1;
    systemInfo.usedBlocks = systemInfo.fatSize;

    fileFS.open(name.c_str(), std::ios::binary | std::ios::in | std::ios::out);
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
    //writeSystemInfo();
    writeMetadata();
}

void FileSystem::loadFile(std::string name){
    int nodeID = appendFileInfoToMetadata(name);
    if(nodeID < 0)
        return;
    copyFileDataToFS(nodeID);
    sortMetadata();
}

void FileSystem::displaySystemInfo(){
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
    //TODO

}

void FileSystem::writeMetadata(){
    //TODO
    blockBuffer buf;
    int bufferNumber;
    if(inodes.empty())
        return;
    
    memcpy(buf, &inodes, sizeof(inodes));
    fileFS.seekp(BLOCK_SIZE);
    cout << "struct: " << sizeof(FileNode) << " buffer: " << sizeof(buf) << " node vector: " << sizeof(inodes) << endl;
    

}

int FileSystem::getMemoryPosition(int blocksNumber){
    if(blocksNumber > systemInfo.totalBlocks - systemInfo.usedBlocks){
        return -1;
    }
    // empty or check if begining is available
    else if(inodes.empty() || (inodes.size() > 0 && inodes.front().startBlock - systemInfo.fatSize >= blocksNumber)){
        return systemInfo.fatSize + 1;
    }
    // check place after the only inode
    else if(inodes.size() == 1 && systemInfo.totalBlocks - getNodeEndBlock(inodes.front()) >= blocksNumber){
        return getNodeEndBlock(inodes.front()) + 1;
    }
    else{
        // check places between blocks
        for(unsigned int i = 0; i < inodes.size() - 1; i++){
            if(inodes[i + 1].startBlock - getNodeEndBlock(inodes[i]) >= blocksNumber)
                return getNodeEndBlock(inodes[i]) + 1;
        }
        // check place in the end
        if(systemInfo.totalBlocks - getNodeEndBlock(inodes.back()) >= blocksNumber)
            return getNodeEndBlock(inodes.back()) + 1;
    }
    return 0;
}

int FileSystem::defragmentMemory(){
    // last to do

    return -1;
}

int FileSystem::appendFileInfoToMetadata(std::string name){
    FileNode newNode;
    strcpy(newNode.name, name.c_str());
    newNode.sizeInBytes = file_size(name);
    newNode.blocksNumber = ceil(newNode.sizeInBytes / (float)BLOCK_SIZE);
    newNode.startBlock = getMemoryPosition(newNode.blocksNumber);
    if(newNode.startBlock == -1){
        cout << "Not enough memory to store the file" << endl;
        return -1;
    }
    else if(newNode.startBlock == 0){
        if(defragmentMemory() == -1){
            cout << "Defragmentation failed, unable to load file due to memory fragmentation" << endl;
            return -1;
        }
        newNode.startBlock = getMemoryPosition(newNode.blocksNumber);
    }
    inodes.push_back(newNode);
    return inodes.size() - 1;
}

void FileSystem::copyFileDataToFS(int nodeID){
    //TODO
}

void FileSystem::sortMetadata(){
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
    return node.startBlock + node.blocksNumber;
}