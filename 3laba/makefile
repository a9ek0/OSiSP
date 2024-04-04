CC = gcc
DEBUG_FLAGS = -g -ggdb -std=c11 -W -Wall -Wextra
RELEASE_FLAGS = -c -W -Wall -Wextra
CFLAGS = $(RELEASE_FLAGS)
BUILD_DIR = build/release

ifeq ($(MODE),debug)
    CFLAGS = $(DEBUG_FLAGS)
    BUILD_DIR = build/debug
endif

SRC_FILES = $(wildcard src/*.c)
OBJ_FILES = $(patsubst src/%.c,$(BUILD_DIR)/%.o,$(SRC_FILES))

.PHONY: all clean

all: build_dir $(BUILD_DIR)/main

build_dir:
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/main: $(OBJ_FILES)
	$(CC) $^ -o $@

$(BUILD_DIR)/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf *.o $(BUILD_DIR)

debug: CFLAGS += $(DEBUG_FLAGS)
debug: all

release: CFLAGS += $(RELEASE_FLAGS)
release: all