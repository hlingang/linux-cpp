target=test
src=test.cpp

$(target):$(src)
	g++ -o $@ $^ -std=c++11 -g -ggdb -I.

clean:
	rm -rf $(target)

.PHONY: clean 
