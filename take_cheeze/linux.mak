TARGET = RPG_RT_EMU_2000

LIBS = \
	-lSDL_image -lpng -lz -lSDL_mixer \
	$(shell pkg-config sdl --libs) \
	$(shell pkg-config glu --libs) \
	$(shell pkg-config gl  --libs) \
	$(shell freetype-config --libs) \
	$(shell pkg-config QtGui --libs) \
	-lQtOpenGL

CFLAGS   = \
	-Os -Wall -Werror -Wundef \
	-pg -ggdb \
	$(addprefix -I,$(INCDIR)) \
	$(shell pkg-config sdl --cflags) \
	$(shell pkg-config glu --cflags) \
	$(shell pkg-config gl  --cflags) \
	$(shell freetype-config --cflags) \
	$(shell pkg-config QtGui --cflags)
CXXFLAGS = $(CFLAGS) -fexceptions
# -fno-rtti
LDFLAGS  = -Map=$(TARGET).map --enable-gold

# CC  = gcc
# CXX = g++
LD  = g++

CURRENT_TIME = $(shell date +%F__%H~%M~%S__%Z)


all : $(TARGET)

include objs.mak

.SUFFIXES: .o .mm
.mm.o :
	$(CXX) -c $(CXXFLAGS) $<

$(TARGET) : $(OBJS)
	$(LD) $(CFLAGS) $(addprefix -Wl,,$(LDFLAGS)) $(LIBS) -o $@ $(OBJS)

run : $(TARGET)
	./$(TARGET)

profile : run
	gprof ./$(TARGET) gmon.out > $(TARGET).proStream

debug : $(TARGET)
	gdb $(TARGET)

clean :
	-rm -f $(OBJS) $(TARGET) *.map gmon.out $(TARGET).profile out.txt err.txt

rebuild : clean $(TARGET)
