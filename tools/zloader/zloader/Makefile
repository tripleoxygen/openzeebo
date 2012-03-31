
TARGET := zloader_$(PATCHNAME)

SOURCES := init.S board.c main.c patch/patch_$(PATCHNAME)

CFLAGS := -std=gnu99 -pedantic -g -W -Wall -DARM9
CFLAGS += -march=armv5
CFLAGS += -I../include

LDFLAGS := -nostartfiles -Wl,-Map,$(TARGET).map -L../libboot -L../libc -L../arch_armv6 -L../arch_msm7k -Tboot.ld

BINARY := 1

LIBRARIES = boot_arch_armv6 boot_arch_msm7k boot boot_c 

include ../edam.mk
