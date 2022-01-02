#!/usr/bin/sh

# lib/windows should have the win32 libraries inside
# use msys2 for this

gcc -O3 -static -s $(find . -name "*.c") -Llib/windows -Iinclude -lraylib -lz -lm -lwsock32 -lws2_32 -lglfw3 -lwinpthread -lglew32 -lopengl32 -lglu32 -luser32 -lgdi32 -lole32 -loleaut32 -limm32 -lwinmm -lversion -o tree
