# -*- coding: utf-8 -*-
#!/usr/bin/env python
# python setup.py build_ext --inplace

from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext

ext_modules=[Extension("stego_helper",
              ["stego_helper.pyx"],
              libraries=["m"],
              extra_compile_args = ["-ffast-math"])]

setup(
  name = "stego_helper",
  cmdclass = {"build_ext": build_ext},
  ext_modules = ext_modules
)

# setup(
#     ext_modules = cythonize("stego_helper.pyx")
# )