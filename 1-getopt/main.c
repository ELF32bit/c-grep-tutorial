#include <stdlib.h> // EXIT_SUCCESS, EXIT_FAILURE
#include <unistd.h> // getopt()
#include <stdio.h> // printf()

/* Use typedef to distinguish variables */
typedef int bool;

struct Options {
	bool ignore_case;
	bool match_whole_words;
	char* search_string;
	char* file_name;
};

int main(int argc, char **argv) {
	struct Options options;
	options.ignore_case = 0;
	options.match_whole_words = 0;
	options.search_string = NULL;
	options.file_name = NULL;

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
		options.file_name = argv[optind + 1];
		/* 'optind' + 2, 3, .. for more arguments */
	} else {
		printf("Error: Bad arguments.\n");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
