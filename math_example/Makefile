# Use this option to toggle the cache manager on or off.
WITH_CACHE=yes

EPIPHANY_HOME=/opt/epiphany/esdk

ifeq ($(WITH_CACHE), yes)
CC=${EPIPHANY_HOME}/tools/e-gnu.armv7l.software-caching/bin/epiphany-elf-gcc
else
CC=${EPIPHANY_HOME}/tools/e-gnu.armv7l/bin/epiphany-elf-gcc
endif

ELIBS=${EPIPHANY_HOME}/tools/e-gnu.armv7l/epiphany-elf/lib/libe-lib.a

ifeq ($(WITH_CACHE), yes)
ELIBPICS=${EPIPHANY_HOME}/tools/e-gnu.armv7l.software-caching/epiphany-elf/lib
else
ELIBPICS=${EPIPHANY_HOME}/tools/e-gnu.armv7l/epiphany-elf/lib
endif

EINCS=${EPIPHANY_HOME}/tools/e-gnu.armv7l/epiphany-elf/sys-include

ifeq ($(WITH_CACHE), yes)
CFLAGS= -g -ffast-math -mfp-mode=round-nearest -fpic
else
CFLAGS= -g -ffast-math -mfp-mode=round-nearest
endif

LDFLAGS= -lm

ifeq ($(WITH_CACHE), yes)
LDSCRIPT= fast_cacheman.ldf
else
LDSCRIPT= fast.ldf
endif

.PHONY: all clean

all: e_math_example

ifeq ($(WITH_CACHE), yes)
e_math_example: e_math_example.c cachemanager.s
else
e_math_example: e_math_example.c
endif
	$(CC) $(CFLAGS) -o $@ $^ -T $(LDSCRIPT) $(LDFLAGS) $(ELIBS) -L$(ELIBPICS) -I$(EINCS)

clean:
	rm -f e_math_example *.o
