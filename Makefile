include config.mk

export 32BIT

MODULE_CONFIG= $(foreach module, $(MODULES), $(module)/src/config.mk)

include $(MODULE_CONFIG)

INC_FLAGS= $(foreach incdir, $(INC_DIRS), -I$(incdir))
LIB_FLAGS= $(foreach libdir, $(LIB_DIRS), -L$(libdir))
EXT_LIBS= $(foreach libname, $(LIB_NAMES), -l$(libname))
OBJS= $(patsubst %.cpp, %.o, $(SRCS))

%.o: %.cpp
	$(CPPC) -c $(CPP_FLAGS) $(INC_FLAGS) $< -o $@

all: $(BIN)

$(BIN): $(OBJS)
	$(CPPC) $(CPP_FLAGS) -o $@ $(OBJS) $(LIB_FLAGS) $(EXT_LIBS) -rdynamic

prebuild:
ifndef SQLITE_PATH
	@echo 'SQLITE_PATH not defined!  usage: make SQLITE_PATH=path prebuild'
	@exit 1
endif
	make -C CppUtilities
	make -f sqlitemake.mk SRC_DIR=$(SQLITE_PATH) DEST_DIR=$(SQLITE_DIST)

clean:
	rm -f -R $(DIST) $(OBJS) $(BIN)

cleanall: clean
	make -C CppUtilities clean
	make -f sqlitemake.mk clean DEST_DIR=$(SQLITE_DIST)

release: $(BIN) $(SHARE) README.md
	rm -Rf $(DIST)
	mkdir $(DIST)
	cp $(word 1, $?) $(DIST)/.
	cp $(word 2, $?) $(DIST)/. -R
	cp $(word 3, $?) $(DIST)/.
