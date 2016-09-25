.PHONY: test hello init clean

CC = gcc
CCFLAGS = -c -fno-builtin -nostdlib

all: init lib test

init:
	mkdir -p bin

lib: build archive

build: entry.c stdio.c string.c
	$(CC) $(CCFLAGS) $^
	mv *.o bin/

archive: bin/stdio.o bin/string.o
	ar -rs crt.a  $^

test:
	gcc -c -ggdb -fno-builtin -nostdlib test.c
	ld -static -e crt_entry bin/entry.o test.o crt.a -o bin/test

hello:
	gcc -c -fno-builtin -nostdlib hello.S -o hello.o
	ld -static -e _start hello.o -o bin/hello

clean:
	rm -f *.o *.a
	rm -f bin/*
