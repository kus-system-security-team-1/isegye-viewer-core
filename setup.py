import os, sys

from distutils.core import setup, Extension
from distutils import sysconfig

import pybind11

sfc_module = Extension(
    'isegye_viewer_core_proc_info', sources=['src/proc_info.cpp'],
    include_dirs=[pybind11.get_include()],
    language='c++',
    libraries=['Advapi32', 'Psapi'],
    )

setup(
    name='isegye_viewer_core_proc_info',
    version='1.0',
    description='Python package with isegye_viewer_core C++ extension (PyBind11)',
    ext_modules=[sfc_module],
)