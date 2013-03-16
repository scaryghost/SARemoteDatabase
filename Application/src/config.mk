M_DIR=Application
M_SRCS=Outer.cpp
SRCS:= $(SRCS) $(addprefix $(M_DIR)/src/,$(M_SRCS))
