#include <stdlib.h> // EXIT_SUCCESS, EXIT_FAILURE
#include <stdio.h> // printf()
#include <unistd.h> // getopt()

#include "grep.h"

int main(int argc, char **argv) {
	struct GrepOptions options;
	options.ignore_case = 0;
	options.match_whole_words = 0;
	options.search_string = NULL;
	char* file_name = NULL;

	/* getopt() is commonly used for input options parsing */
	int c;
	while ((c = getopt(argc, argv, "hiw")) != -1) {
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

	/* 'optind' is a global variable from unistd.h */
	/* getopt() sorts 'argv', so non-option arguments follow options */
	if (optind + 1 < argc) {
		options.search_string = argv[optind + 0];
		file_name = argv[optind + 1];
	} else {
		printf("Error: Bad arguments.\n");
		return EXIT_FAILURE;
	}

	return grep_file(file_name, &options);
}
