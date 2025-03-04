# makefile (invoked with 'make') is a basic build system for C language
# CMake and Scons (Python) are proper build systems used by real projects
all:
	clang *.c -lpthread -o grep && \
	./grep -i -w -t 5 "the" \
	../examples/4-genesis.txt \
	../examples/4-exodus.txt \
	../examples/4-leviticus.txt \
	../examples/4-numbers.txt \
	../examples/4-deuteronomy.txt
