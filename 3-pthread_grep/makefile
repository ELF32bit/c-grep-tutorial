# makefile (invoked with 'make') is a basic build system for C language
# CMake and Scons (Python) are proper build systems used by real projects
all:
	clang main.c grep.c threads.c -lpthread -o grep && \
	./grep -i -w "the" ../examples/3-pthread_grep-1.txt ../examples/3-pthread_grep-2.txt
