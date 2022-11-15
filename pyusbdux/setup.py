#!/usr/bin/python3

"""
setup.py file for pyusbdux.py
"""

from setuptools import setup
from setuptools import Extension
import os
from sys import platform

def read(fname):
    return open(os.path.join(os.path.dirname(__file__), fname)).read()

pyusbdux_module = Extension('_pyusbdux',
			     sources=['pyusbdux.i'],
			     extra_compile_args=['-std=c++11','-O3'],
			     libraries=['comedi'],
                             swig_opts=['-c++','-py3','-threads'],
)


setup (name = 'pyusbdux',
       version = '2.0.3',
       author      = "Bernd Porr",
       author_email = "mail@berndporr.me.uk",
       url = "https://github.com/glasgowneuro/usbdux",
       description = 'API for the USB-DUX DAQ boxes for Linux (https://github.com/glasgowneuro/usbdux)',
       long_description=read('README.rst'),
       ext_modules = [pyusbdux_module],
       py_modules = ["pyusbdux"],
       license='GPL v3',
       classifiers=[
          'Intended Audience :: Developers',
          'Operating System :: POSIX',
          'Programming Language :: Python :: 3',
          ]
      )
