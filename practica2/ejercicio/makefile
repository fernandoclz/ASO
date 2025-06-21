SRC = myls.c
OBJ = $(SRC:.c=.o)
BIN = myls
CC = gcc
CFLAGS = -Wall -g

all: $(BIN)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(OBJ) $(BIN)