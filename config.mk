CPPC=g++
DIST=dist/
MODULES=Database
MAIN=Main/src/SARemoteDatabase.cpp
BIN=saremotedatabase

CPP_FLAGS=-std=c++0x -Werror -Wall

SRCS:=$(MAIN)
INC_DIRS:=. /mnt/hdd2/local/share/include
LIB_DIRS:=CppUtilities/dist /mnt/hdd2/local/share/lib
LIB_NAMES:=pthread dl cpputilities sqlite3
