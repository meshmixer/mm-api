#!/usr/bin/env python

"""
setup.py file for mm api
"""

from distutils.core import setup, Extension


mmapi_module = Extension('_mmapi',
                           sources=['mmapi.cpp', '..\\StoredCommands.cpp'],
						   include_dirs=['..\\'],
						   define_macros=[('USING_MM_COMMAND_API','1')]
                           )

setup (name = 'mmapi',
       version = '0.1',
       author      = "rms",
       description = """meshmixer API python wrapper""",
       ext_modules = [mmapi_module],
       py_modules = ["mmapi"],
       )