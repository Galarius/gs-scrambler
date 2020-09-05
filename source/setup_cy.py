# -*- coding: utf-8 -*-
#!/usr/bin/env python
"""
python setup_cy.py build_ext --inplace
or import pyximport; pyximport.install() in source before import

distutils uses the Makefile distutils.sysconfig.get_makefile_filename()
for compiling and linking: a sea of options.

http://docs.python.org/distutils/introduction.html
http://docs.python.org/distutils/apiref.html  20 pages ...
http://stackoverflow.com/questions/tagged/distutils+python
"""

import numpy
from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext

ext_modules = [
    # ---------------------------------
    # Steganography helper methods
    # ---------------------------------
    Extension(name="gs_helper",
              sources=["gs_helper.pyx"],
              libraries=["m"],
              extra_compile_args=["-ffast-math"]),
    # ---------------------------------
    # Steganography c core methods
    # ---------------------------------
    Extension(name="gsc_core",
              sources=["gsc_core_py_wrapper.pyx"],
              include_dirs = [".", numpy.get_include()],
              language="c++",
              extra_compile_args=["-std=c++11", "-Wno-unused-function"],
              ),
]

setup(
  name = "gs",
  cmdclass = {"build_ext": build_ext},
  ext_modules = ext_modules,
  version = '1.0.0',
  description = 'steganography scrambler',
  author = 'galarius',
  author_email = ''
)