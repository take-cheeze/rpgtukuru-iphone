# C++ source code
KUTO_ENGINE_CXX = \
	$(wildcard $(shell echo KutoEngine/kuto/*.cpp)) \
	$(wildcard $(shell echo KutoEngine/kuto/windows/*.cpp))
RPG2KLIB_CXX = \
	$(wildcard $(shell echo rpg2kdevSDK/SDK/*.cpp)) \
	$(wildcard $(shell echo rpg2kdevSDK/SDK/sueLib/*.cpp))
GAME_CXX = \
	$(wildcard $(shell echo rpgtukuru/*.cpp)) \
	$(wildcard $(shell echo rpgtukuru/Classes/*.cpp)) \
	$(wildcard $(shell echo rpgtukuru/Classes/game/*.cpp)) \
	$(wildcard $(shell echo rpgtukuru/Classes/test/*.cpp)) \
	$(wildcard $(shell echo rpgtukuru/windows/*.cpp))

CXX_SRC = $(KUTO_ENGINE_CXX) $(RPG2KLIB_CXX) $(GAME_CXX)
OBJC_SRC =  $(KUTO_ENGINE_OBJC) $(RPG2KLIB_OBJC) $(GAME_OBJC)

rpg2k : $(RPG2KLIB_CXX:.cpp=.o)
kuto : $(KUTO_ENGINE_CXX:.cpp=.o)
game : $(GAME_CXX:.cpp=.o)

OBJS = \
	$(CXX_SRC:.cpp=.o)

INCDIR = \
	./ \
	./KutoEngine \
	./rpg2kdevSDK/SDK \
	./rpgtukuru/Classes \
	./rpgtukuru/Classes/game/ \
	./rpgtukuru/Classes/test/

.SUFFIXES: .cpp .hpp .h

# header file relate
.hpp.cpp :
.h.cpp   :
