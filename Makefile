TARGETS:=cube sierpinski xplot cantor koch

CFLAGS += -std=gnu99 -O0 -g -I/opt/X11/include
LDFLAGS += -lX11 -lm -L/opt/X11/lib

.PHONY: all

all: $(TARGETS)

cube: cube.o projective.o
	$(CC) -o $@ $^ $(LDFLAGS)

koch: koch.o
	$(CC) -o $@ $^ $(LDFLAGS)

sierpinski: sierpinski.o
	$(CC) -o $@ $^ $(LDFLAGS)

xplot: xplot.o projective.o plotting.o
	$(CC) -o $@ $^ $(LDFLAGS)

cantor: cantor.o projective.o
	$(CC) -o $@ $^ $(LDFLAGS)
