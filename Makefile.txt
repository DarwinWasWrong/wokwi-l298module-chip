SOURCES = src/chip-modulel298.chip.c
TARGET  = dist/chip.wasm
TARBALL = chip.gz


.PHONY: all
all: $(TARGET) dist/chip.json

.PHONY: clean dist
clean:
	 

dist:
		rm -fr dist
		mkdir -p dist
	    cp chip.json dist

$(TARGET): dist $(SOURCES) src/wokwi-api.h
	  clang --target=wasm32-unknown-wasi --sysroot /opt/wasi-libc -nostartfiles -Wl,--export-table -Wl,--no-entry -Werror -o $(TARGET) $(SOURCES)

dist/chip.json: dist chip.json
	  cp chip.json dist
	  apk add --update zip

	  rm -f dist/chip.zip
	  zip -9 dist/chip.zip dist/chip.wasm dist/chip.json 
	  apk add gcompat
	   if [ ! -e ./arduino-cli ]; then \
            wget https://downloads.arduino.cc/arduino-cli/arduino-cli_latest_Linux_64bit.tar.gz; \
            tar -xvf arduino-cli_latest_Linux_64bit.tar.gz; \
			 ./arduino-cli core install arduino:avr; \
      fi
	  ./arduino-cli compile -e -b arduino:avr:uno test/blink

.PHONY: test
test: 
	  apk add gcompat
      test/arduino-cli core install arduino:avr
	  test/arduino-cli compile -e -b arduino:avr:uno test/blink