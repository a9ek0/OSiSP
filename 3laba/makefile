CC = gcc
DEBUG_FLAGS = -g -ggdb -std=c11 -Wno-unused-parameter -Wno-unused-variable -W -Wall -Wextra
RELEASE_FLAGS = -W -Wall -Wno-unused-parameter -Wno-unused-variable -std=c11 -pedantic

ifeq ($(MODE),debug)
    CFLAGS = $(DEBUG_FLAGS)
    BUILD_DIR = build/debug
else
    CFLAGS = $(RELEASE_FLAGS)
    BUILD_DIR = build/release
endif

.PHONY: all clean

all: build_dir start

build_dir:
	mkdir -p $(BUILD_DIR)

start: $(BUILD_DIR)/parent $(BUILD_DIR)/child

$(BUILD_DIR)/parent: $(BUILD_DIR)/parent.o
	$(CC) $< -o $@

$(BUILD_DIR)/child: $(BUILD_DIR)/child.o
	$(CC) $< -o $@

$(BUILD_DIR)/parent.o: src/parent.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/child.o: src/child.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)/*.o $(BUILD_DIR)/parent $(BUILD_DIR)/child