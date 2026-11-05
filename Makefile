CFLAGS = -g -O0 -Wall

all:
	cd src && $(MAKE) CFLAGS='$(CFLAGS)'

clean:
	cd src && $(MAKE) clean

.PHONY: all clean
