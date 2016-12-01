TARGETS:=cube sierpinski xplot

CFLAGS+=-std=gnu99 -O0 -g
LDFLAGS+=-lX11 -lm

.PHONY: all

all: $(TARGETS)

cube: cube.o projective.o
	$(CC) -o $@ $^ $(LDFLAGS)

sierpinski: sierpinski.o
	$(CC) -o $@ $^ $(LDFLAGS)

xplot: xplot.o projective.o
	$(CC) -o $@ $^ $(LDFLAGS)
