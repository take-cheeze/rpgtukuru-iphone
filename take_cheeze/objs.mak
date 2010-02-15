$*.cpp : $*.hpp
$*.cpp : $*.h
$*.c : $*.h

KUTO_ENGINE_SRC = \
	$(shell echo KutoEngine/kuto/*.cpp)
RPG2KLIB_SRC = \
	$(shell echo rpg2kLib/*.cpp)
GAME_SRC = \
	$(shell echo rpgtukuru/Classes/*.cpp) \
	$(shell echo rpgtukuru/Classes/game/*.cpp) \
	$(shell echo rpgtukuru/Classes/test/*.cpp) \

ALL_SRC = $(KUTO_ENGINE_SRC) $(RPG2KLIB_SRC) $(GAME_SRC)

OBJS = $(ALL_SRC:.cpp=.o)

INCDIR = \
	./ \
	./KutoEngine \
	./rpgtukuru/Classes \
	./rpgtukuru/Classes/game/ \
	./rpgtukuru/Classes/test/
