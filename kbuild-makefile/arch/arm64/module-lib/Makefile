obj-y +=built-in.o
lib_y := arch_get_arch.o
$(obj-y):$(lib_y) 
	ld -r -o $@ $^
%.o:%.c FORCE
	gcc -c -o $@ $<	-I. -I../ -I$(PWD)/include/
all: $(obj-y)
FORCE:
.PHONY: clean FORCE
clean:
	rm -rf $(obj-y)
	rm -rf *.o