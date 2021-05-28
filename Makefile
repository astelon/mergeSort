CC = gcc
DEBUG_OPTIONS = -O0 -g -fsanitize=address
CFLAGS = -Wall $(DEBUG_OPTIONS)
INCLUDE_DIR = include

SRC := src
OBJ := obj
BIN := bin

HEADERS := $(wildcard $(INCLUDE_DIR)/*.h)
SOURCES := $(wildcard $(SRC)/*.c)
OBJECTS := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCES))
OUTPUT  := mergeSort

.phony: clean
.phony: test

all: $(OBJECTS)
	$(CC) $^ -o $(BIN)/$(OUTPUT) $(DEBUG_OPTIONS)
	@mkdir -p $(BIN)

$(OBJ)/%.o: $(SRC)/%.c $(HEADERS)
	@mkdir -p $(OBJ)
	$(CC) -I $(INCLUDE_DIR) -o $@ -c $< $(CFLAGS)

clean:
	@rm -f $(OBJECTS) $(BIN)/*

test: $(BIN)/$(OUTPUT)
	./$(BIN)/$(OUTPUT)
