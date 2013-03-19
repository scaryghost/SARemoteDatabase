M_DIR=Data
M_SRCS=SqliteDataChannel.cpp AchievementPack.cpp
SRCS:= $(SRCS) $(addprefix $(M_DIR)/src/,$(M_SRCS))
