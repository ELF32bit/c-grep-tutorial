# makefile (invoked with 'make') is a basic build system for C language
# CMake and Scons (Python) are proper build systems used by real projects
all:
	clang -fPIC -shared -o libgrep.so grep.c && \
	python grep.py
