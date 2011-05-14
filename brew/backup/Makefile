
EXEC=backup

CC=arm-none-eabi-gcc
LD=arm-none-eabi-ld

# Insert the correct path for your BREW SDK installation.
# Insira o caminho correto para a instalação de seu BREW SDK.
BREW_DIR=/opt/brew

INCDIRS=$(BREW_DIR)/sdk/inc $(BREW_DIR)/inc
VPATH=$(BREW_DIR)/sdk/src

SRC= $(wildcard *.c)
OBJ= $(SRC:.c=.o)

CFLAGS=-c -I $(INCDIRS) -mlittle-endian -march=armv6 -mcpu=arm1136j-s -mapcs-frame -fno-builtin -ffunction-sections -DDYNAMIC_APP

all: $(EXEC).mod

$(EXEC).mod: $(EXEC).elf
	wine $(BREW_DIR)/bin/elf2mod.exe $^

$(EXEC).elf: AEEModGen.o AEEAppGen.o $(OBJ)
	$(CC) -Xlinker --emit-relocs --cref -T $(BREW_DIR)/bin/elf2mod.x -o $@ $^

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

AEEModGen.o: AEEModGen.c
	$(CC) $(CFLAGS) -o AEEModGen.o $(VPATH)/AEEModGen.c

AEEAppGen.o: AEEAppGen.c
	$(CC) $(CFLAGS) -o AEEAppGen.o $(VPATH)/AEEAppGen.c

clean:
	rm -rf *.elf *.mod *.o *.map
