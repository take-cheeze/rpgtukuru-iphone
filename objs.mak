# BASE_DIR = $(shell pwd)
BASE_DIR = .

# C++ source code
KUTO_ENGINE_CXX = \
	$(wildcard $(BASE_DIR)/KutoEngine/kuto/*.cpp) \
	$(wildcard $(BASE_DIR)/KutoEngine/kuto/windows/*.cpp)
RPG2KLIB_CXX = \
	$(wildcard $(BASE_DIR)/rpg2kdevSDK/SDK/*.cpp) \
	$(wildcard $(BASE_DIR)/rpg2kdevSDK/SDK/sueLib/*.cpp)
GAME_CXX = \
	$(wildcard $(BASE_DIR)/rpgtukuru/*.cpp) \
	$(wildcard $(BASE_DIR)/rpgtukuru/Classes/*.cpp) \
	$(wildcard $(BASE_DIR)/rpgtukuru/Classes/game/*.cpp) \
	$(wildcard $(BASE_DIR)/rpgtukuru/Classes/test/*.cpp) \
	$(wildcard $(BASE_DIR)/rpgtukuru/windows/*.cpp)

CXX_SRC = $(KUTO_ENGINE_CXX) $(RPG2KLIB_CXX) $(GAME_CXX)
OBJC_SRC =  $(KUTO_ENGINE_OBJC) $(RPG2KLIB_OBJC) $(GAME_OBJC)

OBJS = $(CXX_SRC:.cpp=.o)

INCDIR = \
	$(BASE_DIR)/ \
	$(BASE_DIR)/KutoEngine \
	$(BASE_DIR)/rpg2kdevSDK/SDK \
	$(BASE_DIR)/rpgtukuru/Classes \
	$(BASE_DIR)/rpgtukuru/Classes/game/ \
	$(BASE_DIR)/rpgtukuru/Classes/test/

# rpg2k : $(RPG2KLIB_CXX:.cpp=.o)
# kuto  : $(KUTO_ENGINE_CXX:.cpp=.o)
# game  : $(GAME_CXX:.cpp=.o)
