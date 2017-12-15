SOURCES:=kilo.o abuf.o terminal.o syntax.o rowops.o editor.o fileio.o find.o render.o input.o

CFLAGS=-Wall -Wextra -pedantic -std=c99

kilo: $(SOURCES)
	$(CC) $(SOURCES) -o kilo $(CFLAGS)

clean:
	-rm *.o kilo
