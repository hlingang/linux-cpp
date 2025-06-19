target=test
src=test.c

$(target):$(src)
	gcc -o $@ $^ -std=c11 -g -ggdb

clean:
	rm -rf $(target)

.PHONY: clean 
