M_DIR=Database
M_SRCS=SqliteConnection.cpp
SRCS:= $(SRCS) $(addprefix $(M_DIR)/src/,$(M_SRCS))
