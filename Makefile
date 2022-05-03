modules_dir = ./
CC = gcc
CFLAGS = -std=c18 \
  -Wall -Wconversion -Werror -Wextra -Wfatal-errors -Wpedantic -Wwrite-strings \
  -O2 \
  -I$(modules_dir)
LDFLAGS =
vpath %.h $(modules_dir)
vpath %.c $(modules_dir)
objects = options.o reader.o hashtable.o holdall.o ws.o
executable = ws

all: $(executable)

clean:
	$(RM) $(objects) $(executable)

$(executable): $(objects)
	$(CC) $(objects) $(LDFLAGS) -o $(executable)

options.o: options.c options.h
reader.o: reader.c reader.h
hashtable.o: hashtable.c hashtable.h
holdall.o: holdall.c holdall.h
ws.o: ws.c options.h reader.h
