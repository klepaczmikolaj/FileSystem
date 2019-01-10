#include "FileSystem.h"

FileSystem::FileSystem(){

}

FileSystem::~FileSystem(){

}

void FileSystem::createFileSystem(int size, std::string name){
    systemInfo.sizeKB = size;
    systemInfo.name = name;
    systemInfo.totalBlocks = size * KILOBYTE / BLOCK_SIZE;
    // file allocation table size depending on predefined size of File System
    systemInfo.fatSize = 8;

    //first block used for systemInfo
    systemInfo.usedBlocks = 1;
    
    std::ofstream fileFS(name, std::ios::binary);
    fileFS.write((char*)&systemInfo, sizeof(systemInfo));

}