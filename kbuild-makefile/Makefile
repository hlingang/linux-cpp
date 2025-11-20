ARCH=$(shell uname -m)
PWD := $(shell pwd)

include $(PWD)/.config
include $(PWD)/arch/$(ARCH)/Makefile     ## 导入arch相关的Makefile 
## target 发生覆盖的时候，以最后一次定义为准，但是所有的 target 依赖都会生成
## 如果两次指定的target 名称不一样，则最终生成两个目标文件
core-$(CONFIG_GET_ARCH) += module-lib/   ## 导入主模块
core-y += module-a/ module-b/            ## 导入主模块
core-dirs := $(core-y:%/=%/built-in.o)
static-lib := $(PWD)/build/lib/lib.a 
shared-lib := $(PWD)/build/lib/lib.so
target := $(PWD)/build/bin/test

all:$(target) $(static-lib) $(shared-lib)
lib: $(static-lib) $(shared-lib)

$(core-dirs): prepare
	make -C $(@D)

$(target):$(core-dirs) test.o
	gcc -o $@ $^

$(shared-lib): $(core-dirs)
	gcc -shared -o $@ $^

$(static-lib): $(core-dirs)
	ar rcs $@ $^

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
	mkdir -p $(PWD)/build/bin 
	mkdir -p $(PWD)/build/lib

FORCE:

clean:
	rm -rf build/
	$(call clean_sub_dir)

.PHONY: clean FORCE prepare

swig: FORCE
	make -f $(PWD)/Makefile.swig all