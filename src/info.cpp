#include "include/FileSystem.h"
#include "include/ProgramInterface.h"

namespace po = boost::program_options;
using std::cout;
using std::endl;

bool processComandLine(int argc, char *argv[], po::variables_map &variables, std::string &nameFS){
    po::options_description desc ("Allowed options", 1024, 512); 
    po::variables_map vm;
    try{
        desc.add_options() 
            ("help,h", "display help message")
        ;
        desc.add_options()
            ("name,n", po::value<std::string>(&nameFS)->required(), "name of the File System")
            ("catalog,c", "display catalog")
            ("map,m", "display memory map")
            ("sys,s", "display system info")
        ;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
        variables = vm;

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
    std::string nameFS;
    po::variables_map parameters;
    bool success;
    success = processComandLine(argc, argv, parameters, nameFS);
    if(success){
        fileSystem.openFileSystem(nameFS);
        if(parameters.count("catalog"))
            fileSystem.displayCatalog();

        else if(parameters.count("map"))
            fileSystem.displayMemoryMap();

        else if(parameters.count("sys"))
            fileSystem.displaySystemInfo();

        fileSystem.closeFileSystem();
    }
    return 0;
}