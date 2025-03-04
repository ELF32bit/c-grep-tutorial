# makefile (invoked with 'make') is a basic build system for C language
# CMake and Scons (Python) are proper build systems used by real projects
all:
	clang main.c grep.c -o grep && \
	./grep -i -w "the" ../examples/2-advanced_grep.txt
