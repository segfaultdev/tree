#!/usr/bin/sh

gcc -O1 $(find . -name "*.c") -Llib -Iinclude -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o tree
