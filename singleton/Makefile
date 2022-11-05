CC=g++
CUR_DIR=$(shell pwd;)
SRC=$(wildcard $(CUR_DIR)/*.cpp)
OBJ=$(SRC:%.cpp=%.o)
TARGET=out
INC=-I./

all:$(TARGET)
$(TARGET):$(OBJ)
	$(CC) -o $@ $^

%.o:%.cpp
	$(CC) -c -o $@ $< $(INC)

.PHONY:clean
clean:
	rm -rf $(OBJ)
	rm -rf $(TARGET)

