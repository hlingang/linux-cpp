ARCH=$(shell uname -m)
PWD := $(shell pwd)

PYTHON = python3
PYTHON_CONFIG = python3-config
CFLAGS = $(shell $(PYTHON_CONFIG) --cflags)
LDFLAGS = $(shell $(PYTHON_CONFIG) --ldflags)

MODULE_DIR := $(PWD)/swig
MODULE_NAME := swig_lib
MODULE_OBJ :=  $(MODULE_DIR)/_$(MODULE_NAME).so
MODULE_INTERFACE := $(MODULE_DIR)/$(MODULE_NAME).i
PYTHON_INTERFACE := $(MODULE_DIR)/$(MODULE_NAME).py
WRAP_OBJ := $(MODULE_DIR)/$(MODULE_NAME)_wrap.o
WRAP_SRC := $(MODULE_DIR)/$(MODULE_NAME)_wrap.c
SWIG_BUILD := $(PWD)/build/python

CFLAGS += -fPIC

include $(PWD)/.config
include $(PWD)/arch/$(ARCH)/Makefile     ## 导入arch相关的Makefile 
## target 发生覆盖的时候，以最后一次定义为准，但是所有的 target 依赖都会生成
## 如果两次指定的target 名称不一样，则最终生成两个目标文件
core-$(CONFIG_GET_ARCH) += module-lib/   ## 导入主模块
core-y += module-a/ module-b/            ## 导入主模块
core-dirs := $(core-y:%/=%/built-in.o)

$(core-dirs): prepare
	make -C $(@D)

$(WRAP_OBJ):$(WRAP_SRC) FORCE
	gcc -c $(CFLAGS) -o $@ $< -I. -I./include

$(MODULE_OBJ): $(core-dirs) $(WRAP_OBJ)
	gcc -shared $(LDFLAGS) -o $@ $^

all:$(MODULE_OBJ) install
install:
	echo "installing python module..."
	cp $(MODULE_OBJ) $(PYTHON_INTERFACE) $(SWIG_BUILD)

prepare:
	echo "Detected ARCH: $(ARCH)"
	if [ -L $(PWD)/include/asm ]; then \
		unlink include/asm; \
	fi
	ln -s ${PWD}/include/arch/$(ARCH) ${PWD}/include/asm
	swig -python $(MODULE_INTERFACE)
	mkdir -p $(SWIG_BUILD)

FORCE:

.PHONY: FORCE prepare swig install

swig: $(MODULE_OBJ) install

