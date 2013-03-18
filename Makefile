include config.mk

MODULE_CONFIG= $(foreach module, $(MODULES), $(module)/src/config.mk)

include $(MODULE_CONFIG)

INC_FLAGS= $(foreach incdir, $(INC_DIRS), -I$(incdir))
LIB_FLAGS= $(foreach libdir, $(LIB_DIRS), -L$(libdir))
EXT_LIBS= $(foreach libname, $(LIB_NAMES), -l$(libname))
OBJS= $(patsubst %.cpp, %.o, $(SRCS))

%.o: %.cpp
ifndef SQLITE_PATH
	@echo 'SQLITE_PATH not defined!  usage: make SQLITE_PATH=path'
	@exit 1
endif
	$(CPPC) -c $(CPP_FLAGS) $(INC_FLAGS) $< -o $@

all: $(BIN)

$(BIN): $(OBJS)
	$(CPPC) -o $@ $(OBJS) $(INC_FLAGS) $(LIB_FLAGS) $(EXT_LIBS) -rdynamic

prebuild:
	make -C CppUtilities

clean:
	rm -f -R $(DIST) $(OBJS) $(BIN)

release: $(BIN) $(SHARE)
	rm -Rf $(DIST)
	mkdir $(DIST)
	cp $(word 1, $?) $(DIST)/.
	cp $(word 2, $?) $(DIST)/. -R
