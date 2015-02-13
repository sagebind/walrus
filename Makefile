SRC_DIR := src
OBJ_DIR := obj
SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_FILES))
LD_FLAGS :=
CC_FLAGS := -x c -MMD -std=c11 -Wstrict-prototypes

all: bin/walrus

bin:
	mkdir bin

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(OBJ_DIR)
	gcc $(CC_FLAGS) -c -o $@ $<

bin/walrus: bin $(OBJ_FILES)
	gcc $(LD_FLAGS) -o $@ $(OBJ_FILES)

clean:
	rm -R $(OBJ_DIR) bin
