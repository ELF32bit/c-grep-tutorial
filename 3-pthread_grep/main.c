#include <stdlib.h> // EXIT_SUCCESS, EXIT_FAILURE
#include <stdio.h> // printf()
#include <unistd.h> // getopt()

#include "grep.h"

int main(int argc, char **argv) {
	struct GrepOptions options;
	options.ignore_case = 0;
	options.match_whole_words = 0;
	options.search_string = NULL;

	int file_names_length = 0;
	char** file_names = NULL;
	int threads = 2;

	int c;
	while ((c = getopt (argc, argv, "hiwt:")) != -1) {
		switch (c) {
			case 'i':
				options.ignore_case = 1;
				break;
			case 'w':
				options.match_whole_words = 1;
				break;
			case 't': // requires an argument after -t
				threads = atoi(optarg);
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

		/* Extracting multiple input files into an array */
		file_names_length = argc - (optind + 1);
		file_names = malloc(file_names_length * sizeof(char*));
		for (int index = (optind + 1); index < argc; index++) {
			file_names[index - (optind + 1)] = argv[index];
		}
	} else {
		printf("Error: Bad arguments.\n");
		return EXIT_FAILURE;
	}

	return grep_files(file_names, file_names_length, &options);
}
