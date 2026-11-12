CFLAGS = -g -O0 -Wall -Wextra
TARGET = burmaldabe-core

SOURCES = main.c execution.c context.c handling.c parsing.c
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

deps.mk:
	$(CC) -MM $(SOURCES) > $@

ifneq (clean, $(MAKECMDGOALS))
-include deps.mk
endif

clean:
	rm -f $(TARGET) $(OBJECTS) deps.mk

.PHONY: all clean
