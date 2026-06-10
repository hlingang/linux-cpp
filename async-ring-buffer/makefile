
## makefile 里面的每一条shell 命令都是以子进程的方式执行的 ##
## makefile 里面的每一条shell 命令都是以子进程的方式执行的 ##
## makefile 里面的每一条shell 命令都是以子进程的方式执行的 ##
CXX := g++
TARGET := test
CXXFLAGS := -g -ggdb -std=c++17 -I.

all: target 
target:
	$(CXX) $(CXXFLAGS) -o $(TARGET) ring-buffer.cpp ring-buffer-flush.cpp ring-buffer-manager.cpp test.cpp
clean:
	rm -rf test *.bin
	
