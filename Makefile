SOURCES:=$(wildcard src/*.c)
OBJS:=$(SOURCES:.c=.o)

CFLAGS=-Wall -Wextra -pedantic -std=c99

CONFIG_PATH:=$(HOME)/.kilorc

kilo: $(OBJS)
	$(CC) $(OBJS) -o kilo $(CFLAGS)

install: kilo
ifneq ("$(wildcard $(CONFIG_PATH))", "")
	@echo "Config file exists"
else
	@echo "Config file does not exist. Copying now."
	-cp kilorc.sample $(CONFIG_PATH)
endif
	-cp kilo /usr/local/bin/
	@echo "Executable installed at /usr/local/bin/kilo"
	
clean:
	-rm src/*.o kilo
