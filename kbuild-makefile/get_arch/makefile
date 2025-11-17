ARCH := $(shell uname -m)
PWD := $(shell pwd)
obj-y := get_arch.o
obj-y += $(patsubst %.c,%.o, $(obj-y))
target := built-in.o
all:$(target)
$(target):$(obj-y)
	ld -r -o $@ $^

%.o:%.c FORCE
	gcc -c -o $@ $< -I. -I.. -I../include

FORCE:	
.PHONY: clean FORCE
clean:
	rm -rf $(target)
	rm -rf $(obj-y)