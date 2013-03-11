CPPC=g++
DIST=dist/
MODULES=Core Database Net
MAIN=Main/src/SARemoteDatabase.cpp
BIN=saremotedatabase

CPP_FLAGS:=-std=c++0x -Werror -Wall

ifdef DEBUG
    CPP_FLAGS+= -g
endif

SRCS:=$(MAIN)
INC_DIRS:=. /mnt/hdd2/local/share/include
LIB_DIRS:=CppUtilities/dist /mnt/hdd2/local/share/lib
LIB_NAMES:=pthread dl cpputilities sqlite3
