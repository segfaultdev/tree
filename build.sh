#!/usr/bin/sh

gcc -Os -s $(find . -name "*.c") -Llib -Iinclude -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o tree
