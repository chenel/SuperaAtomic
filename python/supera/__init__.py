from . pysupera import *
from . pysupera import test as test

import os

def get_includes():
	return os.path.dirname(__file__) + "/include/"

def get_lib_dir():
	return os.path.dirname(__file__) + "/lib/"

