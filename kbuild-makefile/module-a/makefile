obj-y +=built-in.o
lib_y := lib_a.o helper_a.o
$(obj-y):$(lib_y) 
	ld -r -o $@ $^
%.o:%.c FORCE
	gcc -c -o $@ $<	-I. -I.. -I../include
all: $(obj-y)
FORCE:
.PHONY: clean FORCE
clean:
	rm -rf $(obj-y)
	rm -rf *.o
