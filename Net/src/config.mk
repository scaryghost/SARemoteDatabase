M_DIR=Net
M_SRCS=Message.cpp TcpListener.cpp
SRCS:= $(SRCS) $(addprefix $(M_DIR)/src/,$(M_SRCS))
