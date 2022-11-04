src=test.cpp
target=test

$(target):$(src)
	g++ -o $@ $^ -I./ -I./include -g -ggdb -fPIC

.PHONY:
	clear

all:$(target)
clean:
	rm -rf test
	rm -rf *.json
