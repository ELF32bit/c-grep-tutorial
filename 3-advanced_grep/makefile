# makefile (invoked with 'make') is a basic build system for C language
# CMake and Scons (Python) are proper build systems used by real projects
all:
	clang main.c grep_string.c grep_file.c -o grep && \
	./grep -i -w "the" ../examples/3-advanced_grep.txt
