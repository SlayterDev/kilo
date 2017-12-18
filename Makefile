SOURCES:=$(wildcard src/*.c)
OBJS:=$(SOURCES:.c=.o)

CFLAGS=-Wall -Wextra -pedantic -std=c99

CONFIG_PATH:=$(HOME)/.kilorc

kilo: $(OBJS)
	$(CC) $(SOURCES) -o kilo $(CFLAGS)

install: kilo
ifeq ($(shell test -e $(CONFIG_PATH) && echo -n yes), yes)
	@echo "Config file exists"
else
	@echo "Config file does not exist. Copying now."
	-cp kilorc.sample $(CONFIG_PATH)
endif
	-cp kilo /usr/local/bin/
	@echo "Executable installed at /usr/local/bin/kilo"
	
clean:
	-rm src/*.o kilo
