obj-y +=built-in.o
lib-y := lib_b.o helper_b.o
CFLAGS := -fPIC
$(obj-y):$(lib-y)
	ld -r -o $@ $^
%.o:%.c FORCE
	gcc -c $(CFLAGS) -o $@ $<	-I. -I.. -I../include
all: $(obj-y)
FORCE:
.PHONY: clean FORCE
clean:
	rm -rf $(obj-y)
	rm -rf *.o