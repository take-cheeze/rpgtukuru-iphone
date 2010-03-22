TARGET = RPG_RT_EMU_2000
 
LIBS = -lstdc++ -lsupc++ -lpng -lz -lglut -lGL -lGLU -lpspvfpu -lpsprtc -lm
	# -lpsplibc

  CFLAGS = -O0 -Wall -Werror -fmessage-length=0
CXXFLAGS = $(CFLAGS)
 LDFLAGS = -Wl,-Map=$(TARGET).map --enable-gold

# define for rtti. comment this out if you don't need rtti
CXXFLAGS += -fno-rtti -DRPG2K_USE_RTTI=0
# CXXFLAGS += -DRPG2K_USE_RTTI=1

# define for endian.
CFLAGS += -DRPG2K_IS_LITTLE_ENDIAN
 
BUILD_PRX = 1 
 
EXTRA_TARGETS   = EBOOT.PBP
PSP_EBOOT_TITLE = "RPG_RT"

include objs.mak

PSPSDK = $(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
