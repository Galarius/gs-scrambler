# -*- coding: utf-8 -*-
#!/usr/bin/env python
# python setup.py build_ext --inplace

from distutils.core import setup
from Cython.Build import cythonize

setup(
    ext_modules = cythonize("stego_helper.pyx")
)