#include <stdlib.h> // EXIT_SUCCESS, EXIT_FAILURE
#include <locale.h> // setlocale()
#include <unistd.h> // getopt()
#include <stdio.h> // printf()

#include "grep.h"

int main(int argc, char **argv) {
	struct GrepOptions options;
	options.ignore_case = 0;
	options.match_whole_words = 0;
	char* search_string_argument = NULL;
	options.search_string = NULL;

	setlocale(LC_ALL, "C.UTF8");

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

	char* file_name = NULL;
	if (optind + 1 < argc) {
		search_string_argument = argv[optind + 0];
		file_name = argv[optind + 1];
	} else {
		printf("Error: Bad arguments.\n");
		return EXIT_FAILURE;
	}

	options.search_string = convert_string(search_string_argument);
	if (options.search_string == NULL) {
		printf("Error: Failed converting search string.");
		return EXIT_FAILURE;
	}

	GrepFileResult grep_file_result = grep_file(file_name, &options);
	printf("Matches found: %zu\n", grep_file_result.match_count);
	free(options.search_string);

	return grep_file_result.exit_code;
}
