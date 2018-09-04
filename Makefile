CFLAGS=-std=c++0x -O2 
LDFLAGS=-L/usr/lib/ -I/usr/include/libr/

CFLAGS+=$(shell pkg-config --cflags r_socket)
LDFLAGS+=$(shell pkg-config --libs r_socket)

all:
	g++ predator.cpp population.cpp sandbox.cpp genotype.cpp eval.cpp -o predator $(CFLAGS) $(LDFLAGS)


clean:
	rm -f predator

