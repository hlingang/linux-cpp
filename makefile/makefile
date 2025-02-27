src=src/test.cpp
ARCH := $(shell uname -m)
src += src/arch/asm_$(ARCH)/m.cpp
obj=$(src:.cpp=.o)
target=test

all:clean $(target)

$(target):$(obj)
	g++ -o $@ $^

%.o:%.cpp prepare prepare1 prepare0
	g++ -c -o $@ $< -I. -I./include 

prepare0: FORCE
ifneq ($(ARCH),arm)
	$(warning "[warning] only support arm")
else
	@echo "prepare0"
endif

prepare1:src/test.cpp FORCE
	@echo "prepare1"-$^

prepare:
	@ln -sf $(CURDIR)/include/arch/asm_$(ARCH)/ $(CURDIR)/include/asm


FORCE: 

clean:
	rm -rf $(target)
	rm -rf $(obj)
	@if [ -L include/asm ];\
	then unlink include/asm; \
	fi;

.PHONY:clean prepare prepare0 prepare1 FORCE
