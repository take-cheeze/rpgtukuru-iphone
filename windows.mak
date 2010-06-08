TARGET = RPG_RT_EMU_2000

SYSTEM_NAME = $(shell uname -s)

ifeq ($(SYSTEM_NAME_NAME),MINGW)
MINGWPATH = C:/MinGW
PATH = $(MINGWPATH)/bin
LDFLAGS += -L$(MINGWPATH)/lib
endif

LIBS = -lglut32 -lpng -lz -lglu32 -lopengl32 -liconv.dll -lgdi32

# define for rtti. comment this out if you don't need rtti
# CXXFLAGS += -fno-rtti -DRPG2K_USE_RTTI=0
CXXFLAGS += -DRPG2K_USE_RTTI=1

# define for endian.
ifeq ($(shell uname -p),powerpc)
CFLAGS += -DRPG2K_IS_BIG_ENDIAN
else
CFLAGS += -DRPG2K_IS_LITTLE_ENDIAN
endif

CFLAGS += \
	-O0 -pg -g3 -Wall -Werror -fmessage-length=0 \
	$(addprefix -I,$(INCDIR))
CXXFLAGS += $(CFLAGS)
LDFLAGS += -Wl,-Map=$(TARGET).map --enable-gold

ifeq ($(SYSTEM_NAME),CYGWIN)
CFLAGS += --no-cygwin
endif

CC  = gcc
CXX = g++
LD  = g++

all : $(TARGET)

CXX_SRC += \
	$(wildcard $(BASE_DIR)/KutoEngine/kuto/windows/*.cpp) \
	$(wildcard $(BASE_DIR)/rpgtukuru/windows/*.cpp) \

include objs.mak
DEPENDS = $(subst .o,.d,$(OBJS))

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
	-$(RM) $(OBJS) $(DEPENDS) $(TARGET).exe $(TARGET).map $(TARGET).profile gmon.out

rebuild : clean $(TARGET)

game  : $(GAME_CXX:.cpp=.o)
kuto  : $(KUTO_ENGINE_CXX:.cpp=.o)
rpg2k : $(RPG2KLIB_CXX:.cpp=.o)


ifneq "$(MAKECMDGOALS)" "clean"
include $(DEPENDS)
endif

%.d: %.cpp
	$(CXX) $(CXXFLAGS) $(TARGET_ARCH) -M $< | \
	sed 's,\($(notdir $*)\.o\) *:,$(dir $@)\1 $@: ,' >$@.tmp
	mv $@.tmp $@
