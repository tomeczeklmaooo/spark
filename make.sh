#!/bin/bash

if [ ! -d "bin" ]; then
	mkdir bin
fi

# add -fsanitize=address,integer,undefined to check for memory leaks and undefined behavior and stuff
clang src/*.c -o bin/spark \
	-Wall -Wextra -Werror \
	-Wshadow -Wpedantic -Wconversion -Wsign-conversion \
	-Wstrict-prototypes -Wmissing-prototypes \
	-Wold-style-definition -Wfloat-equal -Wundef -Wcast-align \
	-Wbad-function-cast -Winline -Wmissing-declarations \
	-std=c23 -ferror-limit=0 -fdiagnostics-color=always
