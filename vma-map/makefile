user:
	g++ -o user user-vma-map.cpp -g -ggdb -std=c++11 -I. -lpthread
kernel:
	g++ -o kernel kernel-vma-map.cpp -g -ggdb -std=c++11 -I. -lpthread
all: user kernel

.PHONY: clean

clean:
	rm -rf test user kernel