CFLAGS=-std=c++0x -O2  #-g
LDFLAGS=-L/usr/lib/ -I/usr/include/libr/

CFLAGS+=$(shell pkg-config --cflags r_socket)
LDFLAGS+=$(shell pkg-config --libs r_socket)

all:
	#TODO: dont build all in one shot
	g++ predator.cpp population.cpp sandbox.cpp r2.cpp genotype.cpp eval.cpp -o predator $(CFLAGS) $(LDFLAGS)


clean:
	rm -f predator

