all:
	g++ -o test test.cpp -g -ggdb -std=c++11 -ffunction-sections -fdata-sections -Wl,--gc-sections -I. -lpthread

.PHONY: clean

clean:
	rm -rf test