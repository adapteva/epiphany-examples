# Use this option to toggle the cache manager on or off.
WITH_CACHE=yes

EPIPHANY_HOME ?= /opt/adapteva/esdk
CROSS_COMPILE ?= arm-linux-gnueabihf-

HOST_ELIBS=$(EPIPHANY_HOME)/tools/host/lib
HOST_EINCS=$(EPIPHANY_HOME)/tools/host/include

ifeq ($(ARCH), armv7l)
	HOST_CC := gcc
else
	HOST_CC := $(CROSS_COMPILE)gcc
endif
HOST_CFLAGS ?= -g
HOST_LDFLAGS ?= -le-hal -le-loader

TARGET_CC ?= epiphany-elf-gcc
TARGET_OBJCOPY ?= epiphany-elf-objcopy

TARGET_ELIBS=${EPIPHANY_HOME}/tools/e-gnu/epiphany-elf/lib/libe-lib.a

TARGET_EINCS=${EPIPHANY_HOME}/tools/e-gnu/epiphany-elf/sys-include

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

all: math_test

math_test: math_test.c
	$(HOST_CC) $(HOST_CFLAGS) -o $@ $< -I$(HOST_EINCS) -L$(HOST_ELIBS) $(HOST_LDFLAGS)


ifeq ($(WITH_CACHE), yes)
e_math_test: e_math_test.c cachemanager.S
else
e_math_test: e_math_test.c
endif
	$(TARGET_CC) $(TARGET_CFLAGS) -o $@ $^ -T $(LDSCRIPT) $(TARGET_LDFLAGS) $(TARGET_ELIBS)

clean:
	rm -f e_math_test math_test *.o
