# Use this option to toggle the cache manager on or off.
WITH_CACHE=yes

EPIPHANY_HOME=/opt/epiphany/esdk

HOST_ELIBS=$(EPIPHANY_HOME)/tools/host/lib
HOST_EINCS=$(EPIPHANY_HOME)/tools/host/include

HOST_CC = arm-linux-gnueabihf-gcc
HOST_CFLAGS = -g
HOST_LDFLAGS = -le-hal -le-loader

ifeq ($(WITH_CACHE), yes)
TARGET_CC=${EPIPHANY_HOME}/tools/e-gnu.armv7l.software-caching/bin/epiphany-elf-gcc
TARGET_OBJCOPY=${EPIPHANY_HOME}/tools/e-gnu.armv7l.software-caching/bin/epiphany-elf-objcopy
else
TARGET_CC=${EPIPHANY_HOME}/tools/e-gnu.armv7l/bin/epiphany-elf-gcc
TARGET_OBJCOPY=${EPIPHANY_HOME}/tools/e-gnu.armv7l/bin/epiphany-elf-objcopy
endif

TARGET_ELIBS=${EPIPHANY_HOME}/tools/e-gnu.armv7l/epiphany-elf/lib/libe-lib.a

ifeq ($(WITH_CACHE), yes)
TARGET_ELIBPICS=${EPIPHANY_HOME}/tools/e-gnu.armv7l.software-caching/epiphany-elf/lib
else
TARGET_ELIBPICS=${EPIPHANY_HOME}/tools/e-gnu.armv7l/epiphany-elf/lib
endif

TARGET_EINCS=${EPIPHANY_HOME}/tools/e-gnu.armv7l/epiphany-elf/sys-include

ifeq ($(WITH_CACHE), yes)
TARGET_CFLAGS= -g -ffast-math -mfp-mode=round-nearest -fpic
else
TARGET_CFLAGS= -g -ffast-math -mfp-mode=round-nearest
endif

TARGET_LDFLAGS= -lm

ifeq ($(WITH_CACHE), yes)
LDSCRIPT= fast_cacheman.ldf
else
LDSCRIPT= fast.ldf
endif

.PHONY: all clean

all: e_math_example.srec math_example

math_example: math_example.c
	$(HOST_CC) $(HOST_CFLAGS) -o $@ $< -I$(HOST_EINCS) -L$(HOST_ELIBS) $(HOST_LDFLAGS)


ifeq ($(WITH_CACHE), yes)
e_math_example: e_math_example.c cachemanager.S
else
e_math_example: e_math_example.c
endif
	$(TARGET_CC) $(TARGET_CFLAGS) -o $@ $^ -T $(LDSCRIPT) $(TARGET_LDFLAGS) $(TARGET_ELIBS) -L$(TARGET_ELIBPICS) -I$(TARGET_EINCS)

e_math_example.srec: e_math_example
	$(TARGET_OBJCOPY) --srec-forceS3 --output-target srec $< $@

clean:
	rm -f e_math_example math_example *.o
