import os, sys

from distutils.core import setup, Extension
from distutils import sysconfig

sfc_module = Extension(
    'isegye_viewer_core_proc_info', sources=['src/proc_info.cpp'],
    include_dirs=['pybind11/include'],
    language='c++',
    )

setup(
    name='isegye_viewer_core_proc_info',
    version='1.0',
    description='Python package with isegye_viewer_core C++ extension (PyBind11)',
    ext_modules=[sfc_module],
)