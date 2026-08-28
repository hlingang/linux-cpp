PYTHON := python3
SWIG := swig	
PY_CFLAGS := $(shell $(PYTHON)-config --cflags)
PY_LDFLAGS := $(shell $(PYTHON)-config --ldflags)
INCLUDE_FLAGS := -I./include

SRC := $(wildcard src/*.cpp src/*.c)
OBJS := $(filter %.o, $(SRC:.cpp=.o) $(SRC:.c=.o))

SWIG_FILE := swig/testlib.i
SWIG_SRC_WRAPPER := python/cxx/testlib_wrap.cxx
SWIG_SRC_WRAPPER_OBJ := $(SWIG_SRC_WRAPPER:.cxx=.o)
SWIG_TARGET := python/cxx/_testlib.so

$(SWIG_SRC_WRAPPER): $(SWIG_FILE) FORCE
	$(SWIG) -python -c++ $(INCLUDE_FLAGS) -o $@ $<

$(SWIG_SRC_WRAPPER_OBJ): $(SWIG_SRC_WRAPPER) FORCE
	$(CXX) -c -fPIC $(PY_CFLAGS) $(INCLUDE_FLAGS) $< -o $@

%.o: %.cpp FORCE
	$(CXX) -c -fPIC $(INCLUDE_FLAGS) $< -o $@

%.o: %.c FORCE
	$(CC) -c -fPIC $(INCLUDE_FLAGS) $< -o $@

$(SWIG_TARGET): $(SWIG_SRC_WRAPPER_OBJ) $(OBJS) 
	$(CXX) -shared $(PY_CFLAGS) $(INCLUDE_FLAGS) -o $@ $^

FORCE:

all: $(SWIG_TARGET)

.PHONY: clean FORCE

