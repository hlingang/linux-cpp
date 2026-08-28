#!/usr/bin/python3
import os
import sys

cur_dir = os.path.dirname(os.path.abspath(__file__))
base_dir = os.path.dirname(cur_dir)
py_dir = os.path.join(base_dir, "python")
if py_dir not in sys.path:
    sys.path.append(py_dir)
    
import testlib

testlib.swig_test(99)