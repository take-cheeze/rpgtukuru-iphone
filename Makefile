TARGET = RPG_RT_EMU_2000

SYSTEM_NAME = $(shell uname -s)

ifeq ($(SYSTEM_NAME_NAME),MINGW)
MINGWPATH = C:/MinGW
PATH = $(MINGWPATH)/bin
LDFLAGS += -L$(MINGWPATH)/lib
endif

LIBS = -lpng -lz -lglut -lGLU -lGL \
	-lSDL_mixer -lSDL \
	$(shell freetype-config --libs) \

# define for rtti. comment this out if you don't need rtti
# CXXFLAGS += -fno-rtti -DRPG2K_USE_RTTI=0
# CXXFLAGS += -DRPG2K_USE_RTTI=1

# define for endian.
# ifeq ($(shell uname -p),powerpc)
# CFLAGS += -DRPG2K_IS_BIG_ENDIAN
# else
# CFLAGS += -DRPG2K_IS_LITTLE_ENDIAN
# endif

CFLAGS += \
	-O0 -pg -g3 -Wall -Werror -fmessage-length=0 \
	$(addprefix -I,$(INCDIR)) \
	$(shell freetype-config --cflags) \

CXXFLAGS += $(CFLAGS) -include "rpgtukuru/Config.hpp"
LDFLAGS += -Wl,-Map=$(TARGET).map --enable-gold

ifeq ($(SYSTEM_NAME),CYGWIN)
CFLAGS += --no-cygwin
endif

LD = $(CXX)

all : $(TARGET)

CXX_SRC += \
	$(wildcard $(BASE_DIR)/KutoEngine/kuto/others/*.cpp) \
	$(wildcard $(BASE_DIR)/rpgtukuru/others/*.cpp) \

include objs.mak

$(TARGET) : $(OBJS)
	$(LD) $(CXXFLAGS) $(LDFLAGS) -o$@ $(OBJS) $(LIBS)

run : $(TARGET)
	./$(TARGET)

profile : run
	gprof ./$(TARGET) gmon.out > $(TARGET).profile

printCurrentTime :
	@echo $(shell date +%F__%H~%M~%S__%Z)

debug : $(TARGET)
	gdb $(TARGET)

clean :
	-$(RM) $(OBJS) $(TARGET) $(TARGET).map $(TARGET).profile gmon.out

rebuild : clean $(TARGET)

windows :
	LDFLAGS="-lwine" CXXFLAGS="-I/usr/include/wine/windows/" make -f windows.mak

game  : $(GAME_CXX:.cpp=.o)
kuto  : $(KUTO_ENGINE_CXX:.cpp=.o)
rpg2k : $(RPG2KLIB_CXX:.cpp=.o)

check_leak : $(TARGET)
	valgrind --tool=memcheck --leak-check=full --show-reachable=yes ./$(TARGET)
