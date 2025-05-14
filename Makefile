# MSYS2
CC        := gcc
#CPPFLAGS  := -D_POSIX_C_SOURCE=200809L
CPPFLAGS := -D_POSIX_C_SOURCE=200809L -DNDEBUG
#CFLAGS    := -Iextern -std=c23 -pedantic -Wall -Wextra -Werror -MMD -MP -g -O0
CFLAGS   := -Iextern -std=c23 -pedantic -Wall -Wextra -MMD -MP -O2
LDFLAGS   := -static -mwindows -lopengl32 -lglfw3

BIN      := break-bricks.exe
MAIN_DIR := src
GAME_DIR := $(MAIN_DIR)/game
GFX_DIR  := $(MAIN_DIR)/gfx
MAIN_SRC := $(wildcard $(MAIN_DIR)/*.c)
GAME_SRC := $(wildcard $(GAME_DIR)/*.c)
GFX_SRC  := $(wildcard $(GFX_DIR)/*.c)
SRC      := $(MAIN_SRC) $(GAME_SRC) $(GFX_SRC)
OBJ      := $(SRC:.c=.o)
DEP      := $(SRC:.c=.d)

ZIP      := break-bricks.zip
IMAGE    := $(wildcard gfx/*.png)
FONT     := $(wildcard font/*.ttf)
LEVEL    := $(wildcard level/*.txt)
MUSIC    := $(wildcard music/*.mp3) 
AUDIO    := $(wildcard sfx/*.wav)
SHADER   := $(wildcard shader/*.glsl)
ZIP_FILE := $(BIN) $(IMAGE) $(FONT) $(LEVEL) $(MUSIC) $(AUDIO) $(SHADER)

all: $(BIN)

zip: $(BIN)
	@rm -f $(ZIP)
	zip $(ZIP) $(ZIP_FILE)

$(BIN): $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

%.o: %.c
	$(CC) -o $@ -c $(CPPFLAGS) $(CFLAGS) $<

-include $(DEP)

clean:
	@rm -f $(BIN) $(OBJ) $(DEP)

run:	all
	@./$(BIN)

.PHONY:	all clean run .zip
