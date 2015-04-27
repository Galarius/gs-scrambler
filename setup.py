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
    # Jonson processing extension class
    # ---------------------------------
    # cython j.pyx -> j.cpp
    # g++ -c j.cpp -> j.o
    # g++ -c jonson_c.cpp -> jonson_c.o
    # link jonson_c.o j.o -> jonson.so
    Extension(name="jonson",
              sources=["j.pyx", "jonson_c.cpp"],
              # extra_objects=["jonson.o"],  # if you compile jonson_c.cpp separately
              include_dirs = [numpy.get_include()],  # .../site-packages/numpy/core/include
              language="c++",
              # libraries=
              # extra_compile_args = "...".split(),
              # extra_link_args = "...".split()
              )
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