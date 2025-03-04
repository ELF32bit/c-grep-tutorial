#include <stdlib.h> // EXIT_SUCCESS, EXIT_FAILURE, atoi(), free()
#include <locale.h> // setlocale()
#include <unistd.h> // getopt()
#include <stdio.h> // printf()

#include "grep.h"

int main(int argc, char **argv) {
	struct GrepOptions options;
	options.ignore_case = 0;
	options.match_whole_words = 0;
	options.search_string = NULL;
	options.available_threads = 1;

	setlocale(LC_ALL, "C.UTF8");

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
				options.available_threads = atoi(optarg);
				if (options.available_threads < 1) {
					options.available_threads = 1;
				}
				break;
			case 'h':
				printf("Search for PATTERN in FILE.\n");
				printf("Usage: grep [OPTIONS] PATTERN FILE\n");
				printf("Example: grep -i 'hello world' main.c\n");
				return EXIT_SUCCESS;
		}
	}

	char** file_names = NULL;
	int file_names_length = 0;
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

	GrepFilesResult grep_files_result = grep_files(file_names, file_names_length, &options);
	printf("Matches found: %zu\n", grep_files_result.match_count);

	free(file_names); // freeing input files array
	return grep_files_result.exit_code;
}
