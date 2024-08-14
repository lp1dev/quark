CFLAGS= -g3 -I . -lSDL2 -lSDL2main -lSDL2_image -lSDL2_ttf

SOURCES=src/*.c

all:
	gcc ${SOURCES} ${CFLAGS} ./liblexbor.so
