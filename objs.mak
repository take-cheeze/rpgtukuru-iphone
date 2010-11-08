# BASE_DIR = $(shell pwd)
BASE_DIR = .

# C++ source code
KUTO_ENGINE_CXX = \
	$(wildcard $(BASE_DIR)/KutoEngine/kuto/*.cpp) \

RPG2KLIB_CXX = \
	$(wildcard $(BASE_DIR)/rpg2kLib/rpg2k/*.cpp) \
	$(wildcard $(BASE_DIR)/rpg2kLib/rpg2k/define/*.cpp) \

GAME_CXX = \
	$(wildcard $(BASE_DIR)/rpgtukuru/*.cpp) \
	$(wildcard $(BASE_DIR)/rpgtukuru/Classes/*.cpp) \
	$(wildcard $(BASE_DIR)/rpgtukuru/Classes/game/*.cpp) \
	$(wildcard $(BASE_DIR)/rpgtukuru/Classes/test/*.cpp) \

#	$(wildcard $(BASE_DIR)/rpgtukuru/windows/*.cpp) \

CXX_SRC += \
	$(KUTO_ENGINE_CXX) $(RPG2KLIB_CXX) $(GAME_CXX) \

OBJS += $(CXX_SRC:.cpp=.o)

INCDIR = \
	$(BASE_DIR) \
	$(BASE_DIR)/KutoEngine \
	$(BASE_DIR)/rpg2kLib/ \
	$(BASE_DIR)/rpgtukuru/Classes \
	$(BASE_DIR)/rpgtukuru/Classes/game/ \
	$(BASE_DIR)/rpgtukuru/Classes/test/ \
	$(BASE_DIR)/rpg2kdevSDK/SDK/ \

# enabling debug flag
CFLAGS += -DRPG2K_DEBUG=1 -DDEBUG
