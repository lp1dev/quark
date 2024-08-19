CFLAGS= -g3 -I . -lSDL2 -lSDL2main -lSDL2_image -lSDL2_ttf -lm

SOURCES=src/*.c src/browser/*.c duktape/src/duktape.c

all:
	gcc ${SOURCES} ${CFLAGS} ./liblexbor.so
