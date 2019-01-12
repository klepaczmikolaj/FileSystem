#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include <fstream>
#include <string>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <cmath>
#include "boost/filesystem.hpp"
#include "boost/program_options.hpp"

#define MAX_FILENAME_SIZE 128
#define KILOBYTE 1024

class FileSystem{
    public:
        FileSystem();
        ~FileSystem();
        void createFileSystem(int size, std::string name);
        void openFileSystem(std::string name);
        void closeFileSystem();
        void loadFile(std::string name);
        void downloadFile(std::string);
        void displayCatalog();
        void deleteFile(std::string);
        void displayMemoryMap();
        void displaySystemInfo();
    private:
        static const unsigned BLOCK_SIZE = 1024; // 1kB
        struct SystemInfo{
            const char fileSystemType[32] = "Contiguous";
            char name[MAX_FILENAME_SIZE];
            int sizeKB;
            int totalBlocks;
            int usedBlocks;
            int fatSize;
            int numberOfFiles;
        }systemInfo;
        
        struct FileNode{
            char name[MAX_FILENAME_SIZE];
            int startBlock;
            int blocksNumber;
            int sizeInBytes;
        };
        std::deque<FileNode> inodes;
        typedef char blockBuffer[BLOCK_SIZE];
        std::fstream fileFS;

        void readSystemInfo(std::string name);
        void readMetadata();
        void writeSystemInfo();
        void writeMetadata();
        int getMemoryPosition(int blocksNumber);
        int defragmentMemory();
        int appendFileInfoToMetadata(std::string name);
        void copyFileDataToFS(int nodeID);
        void writeFileToCurrentDir(int);
        void sortMetadata();
        static bool compareNodes(FileNode node1, FileNode node2);
        void fillSysWithZeros();
        int getNodeEndBlock(FileNode node);
        bool doesFileExistInDIR(std::string);
};


#endif