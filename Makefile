CFLAGS= -g3 -I . -lSDL2 -lSDL2main -lSDL2_image -lSDL2_ttf -lm -Iduktape/src/

SOURCES=src/*.c \
 src/browser/*.c \
  src/browser/classes/*.c \
  src/adapters/*.c \
   duktape/src/duktape.c \
   duktape/extras/console/duk_console.c

all:
	gcc ${SOURCES} ${CFLAGS} ./liblexbor.so
