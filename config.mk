CPPC=g++
DIST=dist
MODULES=Core Database Net
MAIN=Main/src/SARemoteDatabase.cpp
BIN=saremotedatabase
CPPUTILS_LIBNAME:=cpputilities

CPP_FLAGS:=-std=c++0x -Werror -Wall

ifdef DEBUG
    CPP_FLAGS+= -g
    CPPUTILS_LIBNAME:=$(CPPUTILS_LIBNAME)_debug
endif

SRCS:=$(MAIN)
INC_DIRS:=. $(SQLITE_PATH)
LIB_DIRS:=CppUtilities/dist $(SQLITE_PATH)
LIB_NAMES:=pthread dl $(CPPUTILS_LIBNAME) sqlite3
