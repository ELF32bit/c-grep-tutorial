#include <stdlib.h> // for EXIT_SUCCESS, EXIT_FAILURE syntax
#include <stdio.h> // for printf()
#include <unistd.h> // for getopt() argument parsing

#include "grep.h"

int main(int argc, char **argv) {
	struct GrepOptions options;
	options.ignore_case = 0;
	options.match_whole_words = 0;
	options.search_string = NULL;
	char* file_name = NULL;

	//1. getopt() is commonly used for basic input options parsing
	char c;
	while ((c = getopt (argc, argv, "hiw")) != -1) {
		switch (c) {
			case 'i':
				options.ignore_case = 1;
				break;
			case 'w':
				options.match_whole_words = 1;
				break;
			case 'h':
				printf("Search for PATTERN in FILE.\n");
				printf("Usage: grep [OPTIONS] PATTERN FILE\n");
				printf("Example: grep -i 'hello world' main.c\n");
				return EXIT_SUCCESS;
		}
	}

	//2. 'optind' is a global variable from unistd.h
	// getopt() sorts 'argv', so non-option arguments follow option arguments
	if (optind + 1 < argc) {
		options.search_string = argv[optind + 0];
		file_name = argv[optind + 1];
	} else {
		printf("Error: Bad arguments.\n");
		return EXIT_FAILURE;
	}

	//3. printing options.
	printf("Searching in: %s\n", file_name);
	printf("Searching for: %s\n", options.search_string);
	printf("Ignoring case: %s\n", options.ignore_case ? "TRUE" : "FALSE");
	printf("Matching whole words: %s\n", options.match_whole_words ? "TRUE" : "FALSE");

	return grep_file(file_name, options);
}
