SOURCES = src/chip-modulel298.chip.c
TARGET  = dist/chip.wasm
TARBALL = chip.gz


.PHONY: all
all: $(TARGET) dist/chip.json

.PHONY: clean dist
clean:
		rm -rf zip

dist:

		mkdir -p dist
		mkdir -p zip
	    cp chip.json dist
		cp chip.json zip
		cp $(TARGET) zip 
	    tar -czvf  $(TARBALL)  dist/chip.wasm dist/chip.json
		mv $(TARBALL)  dist/chip.zip

$(TARGET): dist $(SOURCES) src/wokwi-api.h
	  clang --target=wasm32-unknown-wasi --sysroot /opt/wasi-libc -nostartfiles -Wl,--import-memory -Wl,--export-table -Wl,--no-entry -Werror -o $(TARGET) $(SOURCES)

dist/chip.json: dist chip.json
	  cp chip.json dist

.PHONY: test
test:
	  cd test && arduino-cli compile -e -b arduino:avr:uno blink