target=test
src=test.cpp

$(target):$(src)
	g++ -o $@ $^ -I./

clean:
	rm -rf $(target)

all:$(target)
.PHONY:clean
