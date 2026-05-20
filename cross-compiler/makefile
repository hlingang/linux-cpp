
## makefile 里面的每一条shell 命令都是以子进程的方式执行的 ##
## makefile 里面的每一条shell 命令都是以子进程的方式执行的 ##
## makefile 里面的每一条shell 命令都是以子进程的方式执行的 ##
CXX := g++
TARGET := test
CXXFLAGS := -g -ggdb -std=c++17 -I.

all:
	$(CXX) $(CXXFLAGS) -o $(TARGET) test.cpp

.PHONY: clean prepare

cross-ndk32:
	export NDK_HOME=~/android-ndk-r26d; \
	export PATH=$$NDK_HOME/toolchains/llvm/prebuilt/linux-x86_64/bin:$$PATH;\
	$(MAKE) \
	CXX=armv7a-linux-androideabi21-clang++ \
	TARGET=$(TARGET)-ndk32 \
	CXXFLAGS="$(CXXFLAGS) -static -D__NDK32__" \
	all

cross-ndk:
	export NDK_HOME=~/android-ndk-r26d;\
	export PATH=$$NDK_HOME/toolchains/llvm/prebuilt/linux-x86_64/bin:$$PATH;\
	$(MAKE) \
	CXX=aarch64-linux-android21-clang++ \
	TARGET=$(TARGET)-ndk \
	CXXFLAGS="$(CXXFLAGS) -static -D__NDK__" \
	all
cross-win32: 
	$(MAKE) \
	CXX=i686-w64-mingw32-g++ \
	TARGET=$(TARGET)-win32.exe \
	CXXFLAGS="$(CXXFLAGS) -static -static-libgcc -static-libstdc++ -D__WIN32__" \
	all 
cross-win: 
	$(MAKE) \
	CXX=x86_64-w64-mingw32-g++ \
	TARGET=$(TARGET)-win.exe \
	CXXFLAGS="$(CXXFLAGS) -static -static-libgcc -static-libstdc++ -D__WIN64__" \
	all 

m32:
	$(MAKE) \
	TARGET=$(TARGET)-32 \
	CXXFLAGS="$(CXXFLAGS) -m32" \
	all
clean:
	rm -rf test
	rm -rf test-ndk32 test-ndk test-win32.exe test-win.exe test-32
