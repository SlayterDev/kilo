SOURCES:=kilo.o abuf.o terminal.o syntax.o rowops.o editor.o fileio.o find.o render.o input.o

CFLAGS=-Wall -Wextra -pedantic -std=c99

CONFIG_PATH:=$(HOME)/.kilorc

kilo: $(SOURCES)
	$(CC) $(SOURCES) -o kilo $(CFLAGS)

install: kilo
ifeq ($(shell test -e $(CONFIG_PATH) && echo -n yes), yes)
	@echo "Config file exists"
else
	@echo "Config file does not exist. Copying now."
	-cp kilorc.sample $(CONFIG_PATH)
endif

clean:
	-rm *.o kilo
