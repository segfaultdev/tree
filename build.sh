#!/usr/bin/sh

gcc -O3 -s $(find . -name "*.c") -Llib -Iinclude -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o tree
