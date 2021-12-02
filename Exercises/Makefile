
SOURCES=$(wildcard *.c)
BIN=$(SOURCES:%.c=%)
OBJ=$(SOURCES:%.c=%.o)
CC=gcc
CFLAGS=-g
LIBS=-lpthread

all: $(BIN)

%.o: %.c Makefile
	$(CC) $(CFLAGS) -c -o $@ $<

$(BIN): %: %.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)


.PHONY: clean

clean:
	-rm $(BIN) $(OBJ)
