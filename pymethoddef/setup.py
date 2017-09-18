from setuptools import setup, Extension

module = Extension(
    'spam',
    sources=['spam.c'],
)

setup(
    name='Spam',
    version='0.1',
    description='C extension sample',
    ext_modules=[module],
)
