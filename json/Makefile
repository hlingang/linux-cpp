src=test.cpp
target=test


$(target):$(src)
	g++ -o $@ $^ -g -ggdb


all:$(target)

clean:
	rm -rf $(target)

.PHONY: clean
