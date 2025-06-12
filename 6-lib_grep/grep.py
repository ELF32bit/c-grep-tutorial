import os, ctypes

libgrep = ctypes.CDLL(os.path.abspath("libgrep.so"))

class GrepOptions(ctypes.Structure):
	_fields_ = [
		("ignore_case", ctypes.c_int),
		("match_whole_words", ctypes.c_int),
		("search_string", ctypes.c_wchar_p),
		("file_name", ctypes.c_char_p),
	]

libgrep.grep.argtypes = [ctypes.POINTER(GrepOptions)]
libgrep.grep.restype = ctypes.c_int

options = GrepOptions(1, 1,
	ctypes.c_wchar_p("את"),
	ctypes.c_char_p("../examples/3-wchar_grep.txt".encode('utf-8')),
)

print("Exit code:", libgrep.grep(ctypes.byref(options)))
