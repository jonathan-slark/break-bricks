.POSIX:

# MSYS2
CC         = gcc
CPPFLAGS   = -D_POSIX_C_SOURCE=200809L
#CPPFLAGS  = -D_POSIX_C_SOURCE=200809L -DNDEBUG
CFLAGS     = -Iextern -std=c23 -pedantic -Wall -Wextra -g -O0
#CFLAGS    = -Iextern std=c23 -pedantic -Wall -Wextra -O2
LDFLAGS    = -static -mwindows -lopengl32 -lglfw3

BIN = breakbricks.exe
SRC = game.c main.c gfx.c util.c
OBJ = $(SRC:.c=.o)

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

%.o: %.c
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $<

game.o: game.c game.h main.h gfx.h util.h config.h
gfx.o: gfx.c main.h gfx.h util.h
main.o: main.c game.h main.h util.h
util.o: util.c main.h util.h

clean:
	@rm -f $(BIN) $(OBJ)

run:	all
	@./$(BIN)

.PHONY:	all clean run
