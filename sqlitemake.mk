CFLAGS:=
ifeq ($(32BIT), 1)
	CFLAGS+= -m32
endif

CC=gcc
SRC=$(SRC_DIR)/sqlite3.c
OBJ=$(DEST_DIR)/sqlite3.o
HDR=$(SRC_DIR)/sqlite3.h
LIB_NAME=$(DEST_DIR)/libsqlite3.a

all: $(LIB_NAME)

$(OBJ): $(DEST_DIR) $(SRC)
	$(CC) $(CFLAGS) -c $(word 2, $?) -o $@

$(LIB_NAME): $(OBJ)
	ar -cvq $(LIB_NAME) $(OBJ)
	cp $(HDR) $(DEST_DIR)/.

$(DEST_DIR):
	if [ ! -e $@ ]; then \
	    mkdir $@; \
	fi

clean:
	rm -fR $(DEST_DIR) $(OBJ)
