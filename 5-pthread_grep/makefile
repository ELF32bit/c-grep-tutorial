# makefile (invoked with 'make') is a basic build system for C language
# CMake and Scons (Python) are proper build systems used by real projects
all:
	clang *.c -lpthread -o grep && \
	./grep -i -w -t 5 "the" \
	../examples/5-genesis.txt \
	../examples/5-exodus.txt \
	../examples/5-leviticus.txt \
	../examples/5-numbers.txt \
	../examples/5-deuteronomy.txt
