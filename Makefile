SOURCES = src/chip-modulel298.chip.c
TARGET  = dist/chip.wasm
TARBALL = chip.tar.gz
.PHONY: all
all: $(TARGET) dist/chip.json

.PHONY: clean
clean:
		rm -rf dist

dist:

		mkdir -p dist
	    cp chip.json dist
		$(TARBALL) $(TARGET) dist/chip.json  
	    echo bugger
	    tar cvfk chip.gz dist/chip.wasm dist/chip.json
		mv chip.tar.gz dist/chip.zip

$(TARGET): dist $(SOURCES) src/wokwi-api.h
	  clang --target=wasm32-unknown-wasi --sysroot /opt/wasi-libc -nostartfiles -Wl,--import-memory -Wl,--export-table -Wl,--no-entry -Werror -o $(TARGET) $(SOURCES)

dist/chip.json: dist chip.json
	  cp chip.json dist

.PHONY: test
test:
	  cd test && arduino-cli compile -e -b arduino:avr:uno blink