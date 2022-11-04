src=cjson.cpp
target=out

$(target):$(src)
	g++ -o $@ $^ -I./include -g -ggdb -fPIC

.PHONY:
	clear

all:$(target)
clean:
	rm -rf out
	rm -rf *.json
