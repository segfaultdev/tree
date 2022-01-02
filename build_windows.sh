#!/usr/bin/sh

# ./windows should have the win32 libraries inside
# use msys2 for this

# TODO: rebuild raylib to use older opengl versions(2.1)

gcc -O3 -static -s $(find . -name "*.c") -Lwindows -Iinclude -lraylib -lz -lm -lwsock32 -lws2_32 -lglfw3 -lwinpthread -lglew32 -lopengl32 -lglu32 -luser32 -lgdi32 -lole32 -loleaut32 -limm32 -lwinmm -lversion -o tree
