#!/usr/bin/sh

echo "WARNING: change PYTHON_PATH inside build.sh before building!"

export PYTHON_PATH=/usr/bin
export PATH="$PYTHON_PATH:$PATH"

gcc -Ofast $(find . -name "*.c") -Llib -Iinclude -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o tree -Wall -Wno-unused-variable -Wno-unused-but-set-variable

emcc -Os $(find . -name "*.c") -Lweb -Iinclude -lraylib -pthread --memory-init-file 0 -s PTHREAD_POOL_SIZE=1 -s EXPORTED_RUNTIME_METHODS=[ccall] -s ASYNCIFY \
  -s USE_GLFW=3 -s TOTAL_MEMORY=67108864 -o tree.html --shell-file web/shell.html -DPLATFORM_WEB -Wall -Wno-unused-variable -Wno-unused-but-set-variable \
  -s BINARYEN_ASYNC_COMPILATION

# removed this cuz slow:
# -s ALLOW_MEMORY_GROWTH=1
