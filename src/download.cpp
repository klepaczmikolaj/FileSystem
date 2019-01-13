#include "include/FileSystem.h"
#include "include/ProgramInterface.h"

namespace po = boost::program_options;
using std::cout;
using std::endl;

bool processComandLine(int argc, char *argv[], std::string &fileName, std::string &nameFS){
    po::options_description desc ("Allowed options", 1024, 512); 
    po::variables_map vm;
    try{
        desc.add_options() 
            ("help,h", "display help message")
        ;
        desc.add_options()
            ("file,f", po::value<std::string>(&fileName)->required(), "specify file name to be downloaded from FS")
            ("name,n", po::value<std::string>(&nameFS)->required(), "name of the File System")
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
    std::string fileName;
    std::string nameFS;
    bool success;
    success = processComandLine(argc, argv, fileName, nameFS);
    if(success){
        fileSystem.openFileSystem(nameFS);
        fileSystem.downloadFile(fileName);
        fileSystem.closeFileSystem();
    }
    return 0;
}