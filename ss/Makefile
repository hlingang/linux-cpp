PWD := $(shell pwd)

include $(PWD)/a/Makefile

target := test

all: $(target)
$(target): test.o
	gcc -o $@ $^
%.o: %.c FORCE
	gcc -c -o $@ $< -I. -I./include

FORCE:
.PHONY: FORCE