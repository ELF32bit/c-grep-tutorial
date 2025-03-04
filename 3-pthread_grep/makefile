# makefile (invoked with 'make') is a basic build system for C language
# CMake and Scons (Python) are proper build systems used by real projects
all:
	clang main.c job_queue.c grep_string.c grep_file.c grep_files.c -lpthread -o grep && \
	./grep -i -w -t 5 "the" \
	../examples/3-genesis.txt \
	../examples/3-exodus.txt \
	../examples/3-leviticus.txt \
	../examples/3-numbers.txt \
	../examples/3-deuteronomy.txt
