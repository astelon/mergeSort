CC = gcc
DEBUG_OPTIONS = -O0 -g -fsanitize=address
CFLAGS = -Wall $(DEBUG_OPTIONS)
INCLUDE_DIR = include

SRC := src
OBJ := obj
SOURCES := $(wildcard $(SRC)/*.c)
OBJECTS := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCES))

.phony: clean
.phony: test

all: $(OBJECTS)
	$(CC) $^ -o bin/mergeSort $(DEBUG_OPTIONS)

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) -I $(INCLUDE_DIR) -o $@ -c $< $(CFLAGS)

clean:
	@rm -f $(OBJS) bin/* obj/*

test: bin/mergeSort
	./bin/mergeSort
