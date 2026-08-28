all:
	g++ -o test test.cpp -g -ggdb -fsanitize=address -std=c++11 -I. -lpthread

.PHONY: clean

clean:
	rm -rf test