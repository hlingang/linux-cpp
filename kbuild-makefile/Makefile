target=test

ARCH=$(shell uname -m)
PWD := $(shell pwd)

include $(PWD)/.config
include $(PWD)/arch/$(ARCH)/Makefile 
core-$(CONFIG_GET_ARCH) += get_arch/
core-y += a/ b/
$(info "**-$(core-y)")
core-dirs := $(core-y:%/=%/built-in.o)

all:$(target)

$(core-dirs): prepare
	make -C $(@D)

$(target):$(core-dirs) test.o
	gcc -o $@ $^
%.o:%.c FORCE
	gcc -c -o $@ $< -I. -I./include

define clean_sub_dir
	for obj_dir in $(core-dirs); do\
		dir_name=$$(dirname $${obj_dir}); \
		echo "clean in $${dir_name}"; \
		make -C $${dir_name} clean; \
	done
	if [ -L $(PWD)/include/asm ]; then \
		unlink include/asm; \
	fi
endef

prepare:
	echo "Detected ARCH: $(ARCH)"
	if [ -L $(PWD)/include/asm ]; then \
		unlink include/asm; \
	fi
	ln -s ${PWD}/include/arch/$(ARCH) ${PWD}/include/asm

FORCE:

clean:
	rm -rf $(target)
	$(call clean_sub_dir)

.PHONY: clean FORCE prepare
