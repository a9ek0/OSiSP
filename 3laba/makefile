CC = gcc
DEBUG_FLAGS = -g -ggdb -std=c11 -pedantic -W -Wall -Wextra
RELEASE_FLAGS = -std=c11 -pedantic -W -Wall -Wextra -Werror

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

$(BUILD_DIR)/parent: $(BUILD_DIR)/parent.o $(BUILD_DIR)/process_management.o $(BUILD_DIR)/signal_handling.o $(BUILD_DIR)/utilities.o
	$(CC) $^ -o $@

$(BUILD_DIR)/child: $(BUILD_DIR)/child.o $(BUILD_DIR)/process_management.o $(BUILD_DIR)/signal_handling.o $(BUILD_DIR)/utilities.o
	$(CC) $^ -o $@

$(BUILD_DIR)/parent.o: src/parent.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/child.o: src/child.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/process_management.o: src/process_management.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/signal_handling.o: src/signal_handling.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/utilities.o: src/utilities.c
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	rm -rf $(BUILD_DIR)/*.o $(BUILD_DIR)/parent $(BUILD_DIR)/child