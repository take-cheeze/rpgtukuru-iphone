TARGET = RPG_RT_EMU_2000

LIBS = -lstdc++ -lsupc++ -lpng -lz -lglut -lGL -lGLU -lpspvfpu -lpsprtc -lm \
	$(shell $(shell psp-config --psp-prefix)/bin/freetype-config --libs) \

  CFLAGS = -O0 -Wall -Werror -fmessage-length=0 -DRPG2K_DEBUG=1 \
	$(shell $(shell psp-config --psp-prefix)/bin/freetype-config --cflags) \

CXXFLAGS = $(CFLAGS) -include "rpgtukuru/Config.hpp"
 LDFLAGS = -Wl,-Map=$(TARGET).map --enable-gold

# define for rtti. comment this out if you don't need rtti
# CXXFLAGS += -fno-rtti -DRPG2K_USE_RTTI=0
# CXXFLAGS += -DRPG2K_USE_RTTI=1

# define for endian.
# CFLAGS += -DRPG2K_IS_LITTLE_ENDIAN

CFLAGS += -O0 -Wall -Werror -fmessage-length=0
LDFLAGS += -W,l-Map=$(TARGET).map --enable-gold

# use PSPSDK's libc (link with -lpsplibc)
# you can't newlib's libc if you use this
#USE_PSPSDK_LIBC = 1

BUILD_PRX = 1

PSP_EBOOT_TITLE = "RPG_RT"

# set for output folder (you have to make output dir beforehand.)
#PSP_EBOOT = $(TARGET)/EBOOT.PBP
PSP_EBOOT = EBOOT.PBP
EXTRA_TARGETS = $(PSP_EBOOT)

CXX_SRC += \
	$(wildcard $(BASE_DIR)/KutoEngine/kuto/others/*.cpp) \
	$(wildcard $(BASE_DIR)/rpgtukuru/others/*.cpp) \

include objs.mak

PSPSDK = $(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

CC  = ccache psp-gcc
CXX = ccache psp-g++
LD  = ccache psp-g++
