from setuptools import Extension, setup

extensions = [Extension('spam', sources=['spam.c'])]

setup(ext_modules=extensions)
