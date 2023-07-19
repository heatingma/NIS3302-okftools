#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Note: To use the 'upload' functionality of this file, you must:
#   $ pipenv install twine --dev

import io
import os
from setuptools import find_packages, setup

NAME = 'okftools'
DESCRIPTION = 'okftools to achieve overloading kernel function under Linux 5.19'
URL = 'https://github.com/heatingma/NIS3302-okftools'
AUTHOR = 'heatingma  qigu  cfg554  halsayxi  sora'

REQUIRED = ['pygame']


here = os.path.abspath(os.path.dirname(__file__))

try:
    with io.open(os.path.join(here, 'README.md'), encoding='utf-8') as f:
        long_description = '\n' + f.read()
except FileNotFoundError:
    long_description = DESCRIPTION

setup(
    name=NAME,
    version='0.0.9',
    description=DESCRIPTION,
    long_description=long_description,
    long_description_content_type='text/markdown',
    author=AUTHOR,
    url=URL,
    packages=['okftools','code_sources'],
    package_data={'okftools': ['*','netlink_kernel/*','GUI/*','user/clean_log/*', 'user/log_control/*', \
                                   'user/receive message/*', 'user/send message/*', 'user/send_control_path/*'], 
                  'code_sources': ['GUI/*', 'kernel/*', 'user/clean_log/*', 'user/log_control/*', \
                                   'user/receive message/*', 'user/send message/*', 'user/send_control_path/*']},
    install_requires=REQUIRED,
    include_package_data=True,
    license='Mulan PSL v2',
    python_requires='>=3.7',
    classifiers=[
        'License :: OSI Approved :: Mulan Permissive Software License v2 (MulanPSL-2.0)',
        'Programming Language :: Python :: 3 :: Only',
        'Operating System :: POSIX :: Linux',
    ],
)
