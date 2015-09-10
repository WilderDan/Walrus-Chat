CC=gcc
CFLAGS=-c -Wall -g3

SOURCES=$(wildcard src/*.c)
OBJECTS=$(notdir $(SOURCES:.c=.o) )

EXECUTABLE=walrus

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)  
	$(CC) $(OBJECTS) -o $(EXECUTABLE) -lpthread

%.o: src/%.c
	$(CC) $(CFLAGS) $< -o $@

.PHONY: clean

clean:
	rm $(OBJECTS) $(EXECUTABLE)
