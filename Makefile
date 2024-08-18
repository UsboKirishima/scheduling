CC = gcc
CFLAGS = -Wall -Wextra -O2
SRC = main.c
EXEC = main

all: $(EXEC)

$(EXEC): $(SRC)
	$(CC) $(CFLAGS) -o $(EXEC) $(SRC)

run: $(EXEC)
	./$(EXEC)

clean:
	rm -f $(EXEC)

.PHONY: all run clean
