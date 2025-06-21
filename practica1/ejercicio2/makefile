SRC = $(filter-out main.c, $(wildcard *.c))
OBJ = $(SRC:.c=.o)
CC = gcc
AR = ar
CFLAGS = -Wall -Wextra -O2

all: appstatic appdynamic

filtro_gris_s.o: filtro_gris.c
	$(CC) $(CFLAGS) -c -o $@ $^

filtro_sepia_s.o: filtro_sepia.c
	$(CC) $(CFLAGS) -c -o $@ $^

ppm_s.o: ppm.c
	$(CC) $(CFLAGS) -c -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -fPIC $<

libppmaso.a: filtro_gris_s.o filtro_sepia_s.o ppm_s.o
	$(AR) cr $@ $^

libppmaso.so: $(OBJ)
	$(CC) -shared -fPIC -o $@ $^

appstatic: libppmaso.a
	$(CC) -o $@ main.c -L. -lppmaso

appdynamic: libppmaso.so
	$(CC) -o $@ main.c -L. -lppmaso -Wl,-rpath,'$$ORIGIN'

clean:
	rm -f *.o appstatic appdynamic libppmaso.so libppmaso.a