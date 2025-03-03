# makefile (invoked with 'make') is a basic build system for C language
# CMake and Scons (Python) are proper build systems used by real projects
all:
	clang main.c grep.c job_queue.c grep_files.c -lpthread -o grep && \
	./grep -i -w -t 2 "the" ../examples/3-pthread_grep-1.txt ../examples/3-pthread_grep-2.txt
