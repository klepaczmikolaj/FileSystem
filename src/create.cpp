#include "include/FileSystem.h"
#include "include/ProgramInterface.h"

namespace po = boost::program_options;
using std::cout;
using std::endl;

bool processComandLine(int argc, char *argv[], int &size, std::string &name){
    po::options_description desc ("Allowed options", 1024, 512); 
    po::variables_map vm;
    try{
        desc.add_options() 
            ("help,h", "display help message")
        ;
        desc.add_options()
            ("size,s", po::value<int>(&size)->required(), "specify size of the FS in kB")
            ("name,n", po::value<std::string>(&name)->required(), "name of the new File System")
        ;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if(vm.count("help") || argc == 1){
            cout << desc;
            return false;
        }
        return true;
    }
    catch(std::exception &e){
        if(vm.count("help") || argc == 1){
            cout << desc;
        }
        else{
            cout << "wrong options, type --help for usage" << endl;
        }
        return false;
    }
}

int main(int argc, char *argv[]){
    FileSystem fileSystem;
    int size;
    std::string name;
    bool success;
    success = processComandLine(argc, argv, size, name);
    if(success)
        fileSystem.createFileSystem(30, name);
    return 0;
}

/*
    FileSystem fileSystem;
    const char *name = "system";
    //fileSystem.createFileSystem(39, name);
    fileSystem.openFileSystem(name);
    //fileSystem.loadFile("file.txt");
    //fileSystem.loadFile("file1.txt");
    //fileSystem.loadFile("file2.txt");
    fileSystem.closeFileSystem();

    fileSystem.displaySystemInfo();

*/