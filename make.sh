#!/bin/bash

if [ ! -d "bin" ]; then
	mkdir bin
fi

gcc src/*.c -o bin/spark -Wall -Wextra -Werror -std=c23