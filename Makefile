CPP_FLAGS := -I /home/mikata/SOI/LAB6/FileSystem -g -Wall -lboost_program_options -lboost_filesystem -lboost_system

all: lib info create download upload delete

info:
	g++ -o bin/info FileSystem.o ProgramInterface.o info.o ${CPP_FLAGS}

create:
	g++ -o bin/create FileSystem.o ProgramInterface.o create.o ${CPP_FLAGS}

download:
	g++ -o bin/download FileSystem.o ProgramInterface.o download.o ${CPP_FLAGS}

upload:
	g++ -o bin/upload FileSystem.o ProgramInterface.o upload.o ${CPP_FLAGS}

delete:
	g++ -o bin/delete FileSystem.o ProgramInterface.o delete.o ${CPP_FLAGS}

lib: src/FileSystem.cpp src/ProgramInterface.cpp src/info.cpp src/create.cpp src/download.cpp src/upload.cpp src/delete.cpp
	g++ -c src/FileSystem.cpp -c src/ProgramInterface.cpp -c src/info.cpp -c src/create.cpp -c src/download.cpp \
	-c src/upload.cpp -c src/delete.cpp ${CPP_FLAGS}
# CXX=       	g++
# CXXFLAGS= 	-I /home/mikata/SOI/LAB6/FileSystem -g -Wall -lboost_program_options -lboost_filesystem -lboost_system
# LDFLAGS=	-Llib
# AR=		ar
# ARFLAGS=	rcs
# 
# LIB_HEADERS=	$(wildcard include/*.h)
# LIB_SOURCE=	$(wildcard src/*.cpp)
# LIB_OBJECTS=	$(LIB_SOURCE:.cpp=.o)
# LIB_STATIC=	lib/libsfs.a
# 
# SHELL_SOURCE=	$(wildcard src/shell/*.cpp)
# 
# all:    create
# 
# %.o:	%.cpp $(LIB_HEADERS)
# 	$(CXX) $< $(CXXFLAGS) -c -o $@ 
# 
# create:		$(LIB_OBJECTS) $(LIB_HEADERS)
# 	g++ -o bin/create src/FileSystem.o src/ProgramInterface.o src/create.o ${CPP_FLAGS}
# create:
# 	g++ -o bin/create FileSystem.o ProgramInterface.o create.o ${CPP_FLAGS}
# 