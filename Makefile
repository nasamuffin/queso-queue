CC=g++
CFLAGS=-I.

DEPS = quesoqueue.h
DEPS += level.h
DEPS += timer.h
DEPS += twitch.h
DEPS += chat.h

OBJ = main.o
OBJ += timer.o
OBJ += twitch.o
OBJ += quesoqueue.o
OBJ += chat.o

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

queso: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
