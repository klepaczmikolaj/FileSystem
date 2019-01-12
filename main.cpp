#include "FileSystem.h"

int main(int argc, char *argv[]){
    FileSystem fileSystem;
    const char *name = "system";
    //fileSystem.createFileSystem(39, name);
    fileSystem.openFileSystem(name);
    //fileSystem.loadFile("file.txt");
    //fileSystem.loadFile("file1.txt");
    //fileSystem.loadFile("file2.txt");
    fileSystem.closeFileSystem();

    fileSystem.displaySystemInfo();
    return 0;
}