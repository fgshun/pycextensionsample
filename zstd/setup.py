from setuptools import setup, Extension

module = Extension(
    'myzstd',
    sources=['myzstd.c'],
    libraries=['zstd'],
)

setup(
    name='Myzstd',
    version='0.1',
    description='use zstd',
    py_modules=['myzstd'],
    ext_modules=[module],
)
