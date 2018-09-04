CFLAGS+=$(shell pkg-config --cflags r_socket)
LDFLAGS+=$(shell pkg-config --libs r_socket)



all:
	gcc test.c -o test -L/usr/lib/ $(CFLAGS) $(LDFLAGS)

