CC=gcc
LD_FLAGS=-lmpd

all:	mpduinod

mpduinod:	mpduinod.c
	$(CC) $(LD_FLAGS) -o $@ $<

clean:
	rm -f mpduinod
