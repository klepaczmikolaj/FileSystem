#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include <fstream>
#include <string>
#include <cstring>
#include <iostream>
#include <iomanip>
#include "boost/filesystem.hpp"
#include "boost/program_options.hpp"

#define MAX_FILENAME_SIZE 256
#define KILOBYTE 1024

class FileSystem{
    static const unsigned BLOCK_SIZE = 1024; // 1kB
    class SystemInfo{
        public:
            const std::string fileSystemType = "Contiguous";
            std::string name;
            int sizeKB;
            int totalBlocks;
            int usedBlocks;
            int fatSize;
    }systemInfo;
    
    class FileNode{
        char name[MAX_FILENAME_SIZE];
        int startBlock;
        int blocksNumber;
        int sizeInBytes;
        int getEndBlock(){
            return startBlock + blocksNumber;
        };
    };

    std::vector<FileNode> nodes;
    typedef char blockBuffer[BLOCK_SIZE];

    public:
        FileSystem();
        ~FileSystem();
        void createFileSystem(int size, std::string name);
        void loadFile(std::string);
        void downloadFile(std::string);
        void displayCatalog();
        void deleteFile(std::string);
        void displayMemoryMap();

};


#endif