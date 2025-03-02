# makefile (invoked with 'make') is a basic build system for C language
# CMake and Scons (Python) are proper build systems used by real projects
all:
	clang main.c grep.c -lpthread -o grep; ./grep -i -w "the" input-1.txt input-2.txt
