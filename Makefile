CC=gcc
LD_FLAGS=-lmpd

all:	mpduinod

mpduinod:	mpduinod.c
	$(CC) $(LD_FLAGS) -o $@ $<

install:	mpduinod
	cp -f mpduinod /usr/bin

clean:
	rm -f mpduinod
