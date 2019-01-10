all: main.cpp FileSystem.cpp FileSystem.h
	g++ main.cpp FileSystem.cpp -oFileSystem -g -Wall -lboost_program_options -lboost_filesystem -lboost_system
