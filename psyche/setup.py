#!/usr/bin/env python
from setuptools import setup, find_packages

setup(
    name='psyche',
    version='0.1.0',
    description=(
        'A DSL that compiles to wired template expressions or numpy code.'
    ),
    author='Joe Jevnik',
    author_email='joejev@gmail.com',
    license='GPL-2',
    classifiers=[
        'Development Status :: 3 - Alpha',
        'License :: OSI Approved :: GNU General Public License v2 (GPLv2)',
        'Natural Language :: English',
        'Programming Language :: Python :: 3.6',
        'Programming Language :: Python :: 3 :: Only',
        'Programming Language :: Python :: Implementation :: CPython',
        'Operating System :: POSIX',
        'Topic :: Software Development :: Pre-processors',
    ],
    url='https://github.com/llllllllll/wired/tree/master/psyche',
    packages=find_packages(),
    install_requires=['numpy'],
)
