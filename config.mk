CPPC=g++
DIST=dist
SHARE=share

CPPUTILS_LIBNAME:=cpputilities
CPP_FLAGS:=-std=c++0x -Werror -Wall

ifdef DEBUG
    CPP_FLAGS+= -g
    CPPUTILS_LIBNAME:=$(CPPUTILS_LIBNAME)_debug
endif
ifeq ($(32BIT), 1)
	CPP_FLAGS+= -m32
else
    32BIT=0
endif

SQLITE_DIST=sqlite3
INC_DIRS:=. $(SQLITE_DIST)
LIB_DIRS:=CppUtilities/dist $(SQLITE_DIST)
LIB_NAMES:=pthread dl $(CPPUTILS_LIBNAME) sqlite3

MAIN=Application/src/main.cpp
SRCS:=$(MAIN)
BIN=saremotedatabase
MODULES=Application Core Data Net

