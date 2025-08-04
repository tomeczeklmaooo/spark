#!/bin/bash

if [ ! -d "bin" ]; then
	mkdir bin
fi

clang src/*.c -o bin/spark \
	-Wall -Wextra -Werror \
	-Wshadow -Wpedantic -Wconversion -Wsign-conversion \
	-Wstrict-prototypes -Wmissing-prototypes \
	-Wold-style-definition -Wfloat-equal -Wundef -Wcast-align \
	-Wbad-function-cast -Winline -Wmissing-declarations \
	-std=c23 -ferror-limit=0 -fdiagnostics-color=always
