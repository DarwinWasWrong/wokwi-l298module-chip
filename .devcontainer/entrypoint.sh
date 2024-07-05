#!/bin/sh

mkdir -p dist
clang --target=wasm32-unknown-wasi --sysroot /opt/wasi-libc -nostartfiles -Wl,--export-table -Wl,--no-entry -Werror -o dist/chip.wasm $SOURCES
