MINGWPATH = C:/MinGW
PATH=${MINGWPATH}/bin
TARGET = RPG_RT_EMU_2000.exe

LIBS = -lglut32 -lpng -lz -lglu32 -lopengl32

CFLAGS = \
	-O0 -g3 -Wall -fmessage-length=0\
	$(addprefix -I,$(INCDIR))
CXXFLAGS = $(CFLAGS)
LDFLAGS  = -L${MINGWPATH}/lib -Map=$(TARGET).map --enable-gold

# CC  = ${MINGWPATH}/bin/gcc
CXX = g++
LD  = g++

# define for rtti. comment this out if you don't need rtti
# CXXFLAGS += -fno-rtti -DRPG2K_USE_RTTI=0
CXXFLAGS += -DRPG2K_USE_RTTI=1

# define for endian.
CFLAGS += -DRPG2K_IS_LITTLE_ENDIAN
# CFLAGS += -DRPG2K_IS_BIG_ENDIAN


all : $(TARGET)

include objs_windows.mak

$(TARGET) : $(OBJS)
	$(LD) -o$@ $(OBJS) $(LIBS)

run : $(TARGET)
	./$(TARGET)

profile : run
	gprof ./$(TARGET) gmon.out > $(TARGET).profile

printCurrentTime :
	@echo $(shell date +%F__%H~%M~%S__%Z)

debug : $(TARGET)
	gdb $(TARGET)

clean :
	$(shell del /Q /S *.o)

rebuild : clean $(TARGET)
