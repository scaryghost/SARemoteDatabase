M_DIR=Data
M_SRCS=SqliteDataChannel.cpp
SRCS:= $(SRCS) $(addprefix $(M_DIR)/src/,$(M_SRCS))
