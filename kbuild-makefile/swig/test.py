#!/bin/python3
import _swig_lib

if __name__ == "__main__":
    size = 32
    buf2 = '\0' * size
    _swig_lib.get_arch(buf2, size)
    print(buf2)

    
    
