TARGET = RPG_RT_EMU_2000

LIBS = \
	-lSDL_image -lpng -lz -lSDL_mixer \
	$(shell pkg-config sdl --libs) \
	$(shell pkg-config glu --libs) \
	$(shell pkg-config gl  --libs) \
	$(shell freetype-config --libs)

CFLAGS   = \
	-Os -Wall -Werror -Wundef \
	-pg -ggdb \
	$(addprefix -I,$(INCDIR)) \
	$(shell pkg-config sdl --cflags) \
	$(shell pkg-config glu --cflags) \
	$(shell pkg-config gl  --cflags) \
	$(shell freetype-config --cflags)
CXXFLAGS = $(CFLAGS) -fexceptions
LDFLAGS  = -Map=$(TARGET).map --enable-gold

CC  = gcc-4.3
CXX = g++-4.3
LD  = g++-4.3

CURRENT_TIME = $(shell date +%F__%H~%M~%S__%Z)


all : $(TARGET)

include objs.mak

$(TARGET) : $(OBJS)
	$(LD) $(CFLAGS) $(addprefix -Wl,,$(LDFLAGS)) $(LIBS) -o $(TARGET) $(OBJS)

run : $(TARGET)
	./$(TARGET)

profile : run
	gprof ./$(TARGET) gmon.out > $(TARGET).proStream

debug : $(TARGET)
	gdb $(TARGET)

clean :
	-rm -f $(OBJS) $(TARGET) *.map gmon.out $(TARGET).profile out.txt err.txt

rebuild : clean $(TARGET)
