
## makefile 里面的每一条shell 命令都是以子进程的方式执行的 ##
## makefile 里面的每一条shell 命令都是以子进程的方式执行的 ##
## makefile 里面的每一条shell 命令都是以子进程的方式执行的 ##
CXX := g++
TARGET := test
CXXFLAGS := -g -ggdb -std=c++17 -I.

all: target 
target:
	$(CXX) $(CXXFLAGS) -o $(TARGET)  test.cpp
clean:
	rm -rf test *.bin *.exe

cross-win: 
	$(MAKE) \
	CXX=x86_64-w64-mingw32-g++ \
	TARGET=$(TARGET)-win.exe \
	CXXFLAGS="$(CXXFLAGS) -static -static-libgcc -static-libstdc++ -D__WIN64__" \
	target 
	
