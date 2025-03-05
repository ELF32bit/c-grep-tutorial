import os, ctypes

grep = ctypes.CDLL(os.path.abspath("libgrep.so"))
grep.example_function.argtypes = [ctypes.c_int]
print(grep.example_function(13))

