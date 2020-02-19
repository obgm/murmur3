CFLAGS=-g -Wall -Wextra -O2

all: murmur3

murmur3:: murmur3.o main.o
