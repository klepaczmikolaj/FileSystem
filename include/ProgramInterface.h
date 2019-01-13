#ifndef PROGRAM_INTERFACE_H
#define PROGRAM_INTERFACE_H

#include "FileSystem.h"

class ProgramInterface{
    public:
        ProgramInterface(int argc, char *argv[], std::string helpMessage);
        ~ProgramInterface();
};

#endif