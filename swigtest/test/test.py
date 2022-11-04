#!/usr/bin/python3
import sys
import os

cur_dir = os.path.dirname(os.path.abspath(__file__))
base_dir = os.path.dirname(cur_dir)
py_dir = os.path.join(base_dir, "python")
if py_dir not in sys.path:
    sys.path.append(py_dir)

import ctestlib

ctestlib.swig_test(10)
ctestlib.swig_test(3.14)
print(ctestlib.cvar.gValue)
a = ctestlib.A()
a.A_test(1)