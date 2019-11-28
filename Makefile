CC=gcc
EXE=main

CFLAGS =
CFLAGS += -g
CFLAGS += -std=c99
CFLAGS += -Wall
CFLAGS += -Wno-unused-function
CFLAGS += -Werror

all: $(EXE)

main: memory.o
main: main.o
	$(CC) $(CFLAGS) $^ -o $(EXE)

memory.o: test.c memory_priv.h memory.h
main.o: memory.h

.PHONY: force
force: clean
force: $(EXE)

.PHONY: run
run: $(EXE)
	./$(EXE)

.PHONY: clean
clean:
	$(RM) $(EXE)
	$(RM) *.o
