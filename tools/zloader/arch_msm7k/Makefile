
TARGET := libboot_arch_msm7k

SOURCES := \
	clock.c \
	hsusb.c \
	mddi_console.c \
	nand.c \
	uart.c \
	gpio.c \
	mddi.c \
	vic.c \
	shared.c \
	smem.c \
	ssbi.c

CFLAGS := -g -W -Wall -DARM9 -DDEBUG
CFLAGS += -march=armv5 -I../include

MKSTATICLIB := 1

include ../edam.mk
