#include "include/FileSystem.h"
#include "include/ProgramInterface.h"

namespace po = boost::program_options;
using std::cout;
using std::endl;

ProgramInterface::ProgramInterface(int argc, char *argv[], std::string helpMessage){
    po::options_description desc ("Allowed options", 1024, 512); 
    desc.add_options() 
        ("help,h", helpMessage.c_str())
    ;
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if(vm.count("help") || argc == 1){
        cout << desc << endl << endl;
    }
}

ProgramInterface::~ProgramInterface(){
    
}