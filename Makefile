SRC_FILES := $(wildcard src/*.c)
OBJ_FILES := $(patsubst src/%.c, obj/%.o, $(SRC_FILES))
LD_FLAGS :=
CC_FLAGS := -x c -MMD -g -std=c99 -Wstrict-prototypes -D_GNU_SOURCE
SCANNER_TESTS := $(wildcard tests/scanner/*)
PARSER_TESTS := $(wildcard tests/parser/*)
SEMANTIC_TESTS := $(wildcard tests/semantics/*.dcf)

.PHONY: all test test-scanner test-parser test-semantics clean

.FORCE:

all: bin/walrus

bin/walrus: bin $(OBJ_FILES)
	gcc $(LD_FLAGS) -o $@ $(OBJ_FILES) $(LIB_FILES)

bin:
	-mkdir bin

obj:
	-mkdir obj

obj/%.o: src/%.c | obj
	gcc $(CC_FLAGS) -c -o $@ $<

test: test-scanner test-parser test-semantics

test-scanner: $(SCANNER_TESTS)

tests/scanner/%: tests/scanner/output/%.out bin/walrus .FORCE
	bin/walrus -s -T $@ | diff -u $< -

test-parser: $(PARSER_TESTS)

tests/parser/legal-%: bin/walrus .FORCE
	bin/walrus -p $@ > /dev/null 2>&1 || bin/walrus $@

tests/parser/illegal-%: bin/walrus .FORCE
	bin/walrus -p $@ > /dev/null 2>&1; test $$? -gt 0

test-semantics: $(SEMANTIC_TESTS)

tests/semantics/legal-%.dcf: bin/walrus .FORCE
	bin/walrus $@ > /dev/null 2>&1 || bin/walrus --debug $@

tests/semantics/illegal-%.dcf: bin/walrus .FORCE
	bin/walrus $@ > /dev/null 2>&1; test $$? -gt 0 || bin/walrus --debug $@; test $$? -gt 0

clean:
	rm -rf obj bin
