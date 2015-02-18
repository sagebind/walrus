SRC_FILES := $(wildcard src/*.c)
OBJ_FILES := $(patsubst src/%.c, obj/%.o, $(SRC_FILES))
LD_FLAGS :=
CC_FLAGS := -x c -MMD -std=c11 -Wstrict-prototypes -D_GNU_SOURCE
SCANNER_TESTS := $(wildcard tests/scanner/*)

all: bin/walrus

bin/walrus: bin $(OBJ_FILES)
	gcc $(LD_FLAGS) -o $@ $(OBJ_FILES) $(LIB_FILES)

bin:
	mkdir bin

obj:
	mkdir obj

obj/%.o: src/%.c obj
	gcc $(CC_FLAGS) -c -o $@ $<

test: $(SCANNER_TESTS)

tests/scanner/%: tests/scanner/output/%.out bin/walrus
	bin/walrus -s -T $@ | diff $< -

clean:
	rm -R obj bin
