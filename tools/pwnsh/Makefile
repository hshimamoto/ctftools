CC = gcc
CFLAGS = -g -O2 -std=c99

all: pwnsh vuln expl

pwnsh: pwnsh.c
	$(CC) -o $@ $(CFLAGS) $<

vuln: vuln.c
	$(CC) -o $@ $(CFLAGS) $<

expl: expl.c
	$(CC) -o $@ $(CFLAGS) $<

clean:
	rm -f pwnsh vuln expl
