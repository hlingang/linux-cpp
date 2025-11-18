target := $(PWD)/a/a
obj := $(PWD)/a/a.o

all: $(target)
$(target): $(obj)
	gcc -o $@ $^
%.o: %.c FORCE
	gcc -c -o $@ $< -I. -I./include

FORCE:
.PHONY: FORCE