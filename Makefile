.POSIX:

# MSYS2
CC         = gcc
CPPFLAGS   = -D_POSIX_C_SOURCE=200809L
#CPPFLAGS  = -D_POSIX_C_SOURCE=200809L -DNDEBUG
CFLAGS     = -Iglad -std=c99 -pedantic -Wall -Wextra -g -O0
#CFLAGS    = -Iglad std=c99 -pedantic -Wall -Wextra -O2
LDFLAGS    = -mwindows -lopengl32 -lglfw3
GLSLC      = glslc
GLSLCFLAGS = --target-env=opengl

BIN = breakbricks.exe
SRC = game.c level.c main.c shader.c sprite.c tex.c
OBJ = $(SRC:.c=.o)

GLSL = shader/sprite_vert.glsl shader/sprite_frag.glsl
SPV  = $(GLSL:.glsl=.spv)

all: $(BIN) $(SPV)

$(BIN): $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

%.o: %.c
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $<

%.spv: %.glsl
	$(GLSLC) $(GLSLCFLAGS) $< -o $@

game.o: game.c config.h game.h shader.h sprite.h tex.h util.h
level.o: level.c util.h
main.o: main.c config.h game.h main.h util.h
shader.o: shader.c main.h shader.h
sprite.o: sprite.c config.h shader.h sprite.h
tex.o: tex.c main.h

clean:
	@rm -f $(BIN) $(OBJ) $(SPV)

run:	all
	@./$(BIN)

.PHONY:	all clean run
