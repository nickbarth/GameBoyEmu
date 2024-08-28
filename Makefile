CC = gcc
CFLAGS = -Wall -I/usr/local/include/raylib
LDFLAGS = -L/usr/local/lib -lraylib
SRCS = $(wildcard src/*.c)
OBJS = $(SRCS:.c=.o)
TARGET = gameboy

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -f src/*.o $(TARGET)
