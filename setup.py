import os, sys

from distutils.core import setup, Extension
from distutils import sysconfig

import pybind11

proc_info_module = Extension(
    'isegye_viewer_core_proc_info', sources=['src/proc_info.cpp'],
    include_dirs=[pybind11.get_include()],
    language='c++',
    libraries=['Advapi32', 'Psapi'],
    )

pe_module = Extension(
    'isegye_viewer_core_pe', sources=['src/pe.cpp'],
    include_dirs=[pybind11.get_include()],
    language='c++',
    libraries=['Advapi32', 'Psapi'],
    )

history_module =  Extension(
    'isegye_viewer_core_history', sources=['src/history.cpp'],
    include_dirs=[pybind11.get_include()],
    language='c++',
    libraries=['Advapi32', 'Psapi'],
    )

setup(
    name='isegye_viewer_core',
    version='1.0',
    description='Python package with isegye_viewer_core C++ extension (PyBind11)',
    ext_modules=[proc_info_module, pe_module, history_module],
)
