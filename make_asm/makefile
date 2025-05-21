BUILD_DIR := build
SRC_DIR := src
ARCH := $(shell uname -m)
$(info "ARCH: $(ARCH)")
include config
#====================================================
target=test
src-y = src/test.cpp
src-$(CONFIG-M-TEST) += src/m_test.cpp
src-y +=  $(wildcard src/arch/asm_$(ARCH)/*.cpp)
src_subdirs = $(shell find $(SRC_DIR) -type d)
build_subdirs = $(src_subdirs:%=$(BUILD_DIR)/%)
$(info "src_subdirs   = $(src_subdirs)")
$(info "build_subdirs = $(build_subdirs)")
#====================================================
obj=$(src-y:%.cpp=build/%.o)
$(info "obj = $(obj)")
#====================================================
all: clean $(target)

$(target):$(obj)
	g++ -o $@ $^
## [内存中] 的 .o 和 .cpp 的依赖关系和 make 规则
build/%.o:%.cpp prepare_subdir prepare_link
	g++ -c -o $@ $< -I. -I./include 

prepare_subdir: FORCE
	mkdir -p $(build_subdirs)
ifneq ($(ARCH),arm)
	@echo $(src)
	$(warning "[warning] only support arm")
else
	@echo "prepare_subdir"
endif

prepare_link:
	@ln -sf $(CURDIR)/include/arch/asm_$(ARCH)/ $(CURDIR)/include/asm


FORCE: 
clean:
	rm -rf $(target)
	rm -rf $(obj)
	rm -rf $(BUILD_DIR)
	@if [ -L include/asm ]; \
	then unlink include/asm;\
	fi;

.PHONY:clean prepare prepare0 prepare1 FORCE