SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c = .o)
CC = gcc
INCLUDES = -I./
CCFLAGS = -g -Wall -O0

TARGET = efipartition

$(TARGET) : $(OBJS)
	$(CC) $^ -o $@ $(INCLUDES)

%.o : %.c
	$(CC) -c $< $(CCFLAGS) 

clean:
	rm $(TARGET)
