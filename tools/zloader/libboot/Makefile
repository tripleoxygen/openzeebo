
TARGET := libboot

SOURCES := \
	flash.c \
	poll.c \
	tags_partition.c \
	tags_revision.c \
	tags_serialno.c \
	tags_cmdline.c \
	gpio_keypad.c \
	init.c \
	tags.c

CFLAGS := -g -W -Wall -DDEBUG
CFLAGS += -march=armv5 -I../include

MKSTATICLIB := 1

include ../edam.mk
