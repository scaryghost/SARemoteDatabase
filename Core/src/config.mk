M_DIR=Core
M_SRCS=Utility.cpp Global.cpp
SRCS:= $(SRCS) $(addprefix $(M_DIR)/src/,$(M_SRCS))
