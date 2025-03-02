#include <stdlib.h> // for EXIT_SUCCESS, EXIT_FAILURE syntax
#include <stdio.h> // for printf()
#include <unistd.h> // for getopt() argument parsing

#include "grep.h"

int main(int argc, char **argv) {
	struct GrepOptions options;
	options.ignore_case = 0;
	options.match_whole_words = 0;
	options.search_string = NULL;
	int file_names_length = 0;
	char** file_names = NULL;

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
		// extracting multiple input files into an array
		file_names_length = argc - (optind + 1);
		file_names = malloc(file_names_length * sizeof(char*));
		for (int index = (optind + 1); index < argc; index++) {
			file_names[index - (optind + 1)] = argv[index];
		}
	} else {
		printf("Error: Bad arguments.\n");
		return EXIT_FAILURE;
	}

	//3. printing options.
	printf("Searching in:");
	for (int index = 0; index < file_names_length; index++) {
		printf(" %s", file_names[index]);
	}
	printf("\n");
	printf("Searching for: %s\n", options.search_string);
	printf("Ignoring case: %s\n", options.ignore_case ? "TRUE" : "FALSE");
	printf("Matching whole words: %s\n", options.match_whole_words ? "TRUE" : "FALSE");

	return grep_files(file_names, file_names_length, &options);
}
