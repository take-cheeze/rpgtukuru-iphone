# C++ source code
KUTO_ENGINE_CXX = \
	$(shell echo KutoEngine/kuto/*.cpp)
RPG2KLIB_CXX = \
	$(shell echo rpg2kLib/*.cpp)
GAME_CXX = \
	$(shell echo rpgtukuru/*.cpp) \
	$(shell echo rpgtukuru/Classes/*.cpp) \
	$(shell echo rpgtukuru/Classes/game/*.cpp) \
	$(shell echo rpgtukuru/Classes/test/*.cpp)

# Objective-C source code
KUTO_ENGINE_OBJC = \
	$(shell echo KutoEngine/kuto/*.mm)
RPG2KLIB_OBJC =
GAME_OBJC = \
	$(shell echo rpgtukuru/Classes/*.mm) \
	$(shell echo rpgtukuru/Classes/game/*.mm) \
	$(shell echo rpgtukuru/Classes/test/*.mm)

CXX_SRC = $(KUTO_ENGINE_CXX) $(GAME_CXX) $(RPG2KLIB_CXX) 
OBJC_SRC =  $(KUTO_ENGINE_OBJC) $(RPG2KLIB_OBJC) $(GAME_OBJC)

OBJS = \
	$(CXX_SRC:.cpp=.o)
#	$(OBJC_SRC:.mm=.o) 

INCDIR = \
	./ \
	./KutoEngine \
	./rpgtukuru/Classes \
	./rpgtukuru/Classes/game/ \
	./rpgtukuru/Classes/test/

.SUFFIXES: .cpp .mm .hpp .h

# header file relate
.hpp.cpp :
.h.cpp   :
.h.mm    :
