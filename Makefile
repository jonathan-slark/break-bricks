.POSIX:

# MSYS2
CC        = gcc
CPPFLAGS = -D_POSIX_C_SOURCE=200809L
#CPPFLAGS  = -D_POSIX_C_SOURCE=200809L -DNDEBUG
CFLAGS   = -Iextern -std=c23 -pedantic -Wall -Wextra -Werror -g -O0
#CFLAGS    = -Iextern -std=c23 -pedantic -Wall -Wextra -O2 -msse2 -mavx2
LDFLAGS   = -static -mwindows -lopengl32 -lglfw3

BIN = breakbricks.exe
SRC = main.c
OBJ = $(SRC:.c=.o)

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

%.o: %.c
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $<

clean:
	@rm -f $(BIN) $(OBJ)

run:	all
	@./$(BIN)

.PHONY:	all clean run
