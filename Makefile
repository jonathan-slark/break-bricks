.POSIX:

# MSYS2
CC        = gcc
CPPFLAGS = -D_POSIX_C_SOURCE=200809L
#CPPFLAGS  = -D_POSIX_C_SOURCE=200809L -DNDEBUG
CFLAGS   = -Iextern -std=c23 -pedantic -Wall -Wextra -Werror -MMD -MP -g -O0
#CFLAGS    = -Iextern -std=c23 -pedantic -Wall -Wextra -MMD -MP -O2
LDFLAGS   = -static -mwindows -lopengl32 -lglfw3

BIN = break-bricks.exe
SRC = font.c game.c gfx.c main.c quad.c rend.c screen.c shader.c tex.c util.c
OBJ = $(SRC:.c=.o)
DEP = $(OBJ:.o=.d)

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

%.o: %.c
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $<

-include $(DEP)

clean:
	@rm -f $(BIN) $(OBJ) $(DEP)

run:	all
	@./$(BIN)

.PHONY:	all clean run
