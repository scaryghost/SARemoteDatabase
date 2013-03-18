CPPC=g++
DIST=dist
SHARE=share

CPPUTILS_LIBNAME:=cpputilities
CPP_FLAGS:=-std=c++0x -Werror -Wall

ifdef DEBUG
    CPP_FLAGS+= -g
    CPPUTILS_LIBNAME:=$(CPPUTILS_LIBNAME)_debug
endif

INC_DIRS:=. $(SQLITE_PATH)
LIB_DIRS:=CppUtilities/dist $(SQLITE_PATH)
LIB_NAMES:=pthread dl $(CPPUTILS_LIBNAME) sqlite3

MAIN=Application/src/main.cpp
SRCS:=$(MAIN)
BIN=saremotedatabase
MODULES=Application Core Data Net
