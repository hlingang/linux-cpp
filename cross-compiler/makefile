
## makefile 里面的每一条shell 命令都是以子进程的方式执行的 ##
## makefile 里面的每一条shell 命令都是以子进程的方式执行的 ##
## makefile 里面的每一条shell 命令都是以子进程的方式执行的 ##
CXX := g++
TARGET := test
CXXFLAGS := -g -ggdb -std=c++17 -I. -lpthread

all:
	$(CXX) $(CXXFLAGS) -o $(TARGET) test.cpp

.PHONY: clean prepare

cross: 
	$(MAKE) \
	CXX=x86_64-w64-mingw32-g++ \
	TARGET=$(TARGET).exe \
	CXXFLAGS="$(CXXFLAGS) -static -static-libgcc -static-libstdc++ -DWIN" \
	all 
clean:
	rm -rf test