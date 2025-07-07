CC = g++
CFLAGS = -g -ggdb -Wall
SRC = test.cpp
OBJ = test.o
TARGET = test
INCFLAGS = -I.

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^

$(OBJ): $(SRC)
	$(CC) $(CFLAGS) $(INCFLAGS) -c -o $@ $< 

.PHONY:clean

clean:
	rm -rf *.o
	rm -rf *.bin
	rm -rf $(TARGET)
	

