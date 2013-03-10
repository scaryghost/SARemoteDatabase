include config.mk

MODULE_CONFIG= $(foreach module, $(MODULES), $(module)/src/config.mk)
INC= $(foreach incdir, $(INC_DIRS), -I$(incdir))
LIB= $(foreach libdir, $(LIB_DIRS), -L$(libdir))
LIBS= $(foreach libname, $(LIB_NAMES), -l$(libname))

include $(MODULE_CONFIG)

OBJS= $(patsubst %.cpp, %.o, $(SRCS))

all: setup $(OBJS)
	$(CPPC) -o $(DIST)/$(BIN) $(OBJS) $(INC) $(LIB) $(LIBS)

%.o: %.cpp
	$(CPPC) -c $(CPP_FLAGS) $(INC) $< -o $@

setup:
	if [ ! -e $(DIST) ]; then \
	    mkdir $(DIST); \
	fi

doc: setup
	doxygen

clean:
	rm -f -R $(DIST) $(OBJS)
