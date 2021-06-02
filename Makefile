CC = gcc
AR := ar -rcs
DEBUG_OPTIONS = -O3
CFLAGS = -Wall $(DEBUG_OPTIONS)
INCLUDE_DIR = include

SRC := src
OBJ := obj
LIB := lib
TST := test

HEADERS := $(wildcard $(INCLUDE_DIR)/*.h)
SOURCES := $(wildcard $(SRC)/*.c)
TESTS   := $(wildcard $(TST)/*.c)
TESTS_BIN := $(patsubst $(TST)/%.c, $(TST)/out/%, $(TESTS)) 
OBJECTS := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCES))
OUTPUT  := libmergesort.a

.phony: clean
.phony: test

all: $(OBJECTS)
	@mkdir -p $(LIB)
	$(AR) $(LIB)/$(OUTPUT) $^

$(OBJ)/%.o: $(SRC)/%.c $(HEADERS)
	@mkdir -p $(OBJ)
	$(CC) -I $(INCLUDE_DIR) -o $@ -c $< $(CFLAGS)

clean:
	@rm -f $(OBJECTS) $(LIB)/*
	@rm -rf $(TST)/out

$(TST)/out/%: $(TST)/%.c
	@mkdir -p $(TST)/out
	$(CC) -I $(INCLUDE_DIR) -o $@ $< $(CFLAGS) --static -L $(LIB) -lmergesort

test: $(TESTS_BIN)
	./$(TST)/run_tests ./$(TST)/out
