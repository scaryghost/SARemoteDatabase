CPPC=g++
DIST=dist/
MODULES=Database
MAIN=Main/src/SARemoteDatabase.cpp
BIN=saremotedatabase

CPP_FLAGS=-std=c++0x -Werror -Wall

SRCS:=$(MAIN)
INC_DIRS:=.
LIB_DIRS:=CppUtilities/dist
LIB_NAMES:=cpputilities
