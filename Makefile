CC = gcc
CFLAGS = -Wall `sdl2-config --cflags` 
LDFLAGS = `sdl2-config --libs`
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
