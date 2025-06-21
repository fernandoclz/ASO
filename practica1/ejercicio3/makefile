CC = gcc
CFLAGS = -Wall -Wextra -O2 -fPIC
LDFLAGS = -shared -fPIC

all: libppmaso.so libfiltro_gris.so libfiltro_sepia.so ppmfilter

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

libppmaso.so: ppm.o
	$(CC) $(LDFLAGS) -o $@ $^

libfiltro_gris.so: filtro_gris.o
	$(CC) $(LDFLAGS) -o $@ $^

libfiltro_sepia.so: filtro_sepia.o
	$(CC) $(LDFLAGS) -o $@ $^

ppmfilter: main.c libppmaso.so
	$(CC) $(CFLAGS) -o $@ $< -L. -lppmaso -Wl,-rpath,'$$ORIGIN'

clean:
	rm -f *.o *.so ppmfilter