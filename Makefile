CC=gcc
LD=gcc -lgcc

CFLAGS=-c -O2 -I./libmsvg/src/
LDFLAGS=-static -O2

all: svg_lib memmapper

memmapper: main.c
	$(CC) $(CFLAGS) main.c -o main.o
	$(LD) $(LDFLAGS) *.o libmsvg/src/libmsvg.a -o MemMapper
	chmod +x MemMapper.exe
	MemMapper.exe test.txt test.svg

svg_lib: libmsvg
	cd libmsvg && $(MAKE)

clean:
	rm -rf *.o
	rm -rf MemMapper