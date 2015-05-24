# -*- coding: utf-8 -*-
#!/usr/bin/env python
# python setup.py build_ext --inplace

# distutils uses the Makefile distutils.sysconfig.get_makefile_filename()
# for compiling and linking: a sea of options.

# http://docs.python.org/distutils/introduction.html
# http://docs.python.org/distutils/apiref.html  20 pages ...
# http://stackoverflow.com/questions/tagged/distutils+python

import numpy
from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext

ext_modules = [
    # ---------------------------------
    # Steganography helper methods
    # ---------------------------------
    Extension(name="stego_helper",
              sources=["stego_helper.pyx"],
              libraries=["m"],
              extra_compile_args = ["-ffast-math"]),
    # ---------------------------------
    # Steganography core methods
    # ---------------------------------
    # cython core_py_wrapper.pyx -> core_py_wrapper.cpp
    # g++ -c core_py_wrapper.cpp -> core_py_wrapper.o
    # g++ -c core.cpp -> core.o
    # link core.o core_py_wrapper.o -> core_py_wrapper.so
    Extension(name="core",
              sources=["core_py_wrapper.pyx", "core.cpp"],
              include_dirs = [numpy.get_include()],
              language="c++",
              extra_compile_args=["-std=c++11", "-Wno-unused-function"],
              # extra_objects=["..."],  # if  compile sources separately
              # libraries=
              # extra_compile_args = "...".split(),
              # extra_link_args = "...".split()
              ),
]

setup(
  name = "stego_helper",
  cmdclass = {"build_ext": build_ext},
  ext_modules = ext_modules
  # version=
  # description=
  # author=
  # author_email=
)