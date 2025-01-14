.POSIX:

# MSYS2
CC         = gcc
CPPFLAGS   = -D_POSIX_C_SOURCE=200809L
#CPPFLAGS  = -D_POSIX_C_SOURCE=200809L -DNDEBUG
CFLAGS     = -Iextern -std=c23 -pedantic -Wall -Wextra -g -O0
#CFLAGS    = -Iextern -std=c23 -pedantic -Wall -Wextra -O2 -msse2 -mavx2
LDFLAGS    = -static -mwindows -lopengl32 -lglfw3

BIN = breakbricks.exe
SRC = aud.c game.c main.c gfx.c util.c
OBJ = $(SRC:.c=.o)

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

%.o: %.c
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $<

aud.o: aud.c main.h aud.h
game.o: game.c main.h aud.h game.h gfx.h util.h config.h
gfx.o: gfx.c gfx.h main.h util.h
main.o: main.c main.h game.h gfx.h
util.o: util.c main.h util.h

clean:
	@rm -f $(BIN) $(OBJ)

run:	all
	@./$(BIN)

.PHONY:	all clean run
