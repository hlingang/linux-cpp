all:
	g++ -o test test.cpp -fsanitize=address -g -ggdb -std=c++11 -I. -lpthread

.PHONY: clean

clean:
	rm -rf test