#include <stdlib.h> // for EXIT_SUCCESS, EXIT_FAILURE syntax
#include <stdio.h> // for printf() and fopen()
#include <unistd.h> // for getopt() argument parsing
#include <string.h> // for manipulating C strings
#include <ctype.h> // for toupper() and isalpha()

// terminal colors
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_RESET "\x1b[0m"

struct Options {
	int ignore_case;
	int match_whole_words;
	char* search_string;
	char* file_name;
};

int grep(struct Options options) {
	// 1. Trying to open input file for reading
	FILE *file = fopen(options.file_name, "r");
	if (file == NULL) {
		printf("Error: No such file.\n");
		return EXIT_FAILURE;
	}

	//2. size_t stores max array size on x64 or x86 system
	size_t search_string_length = strlen(options.search_string);
	char* search_string = options.search_string;
	size_t index = 0;

	// duplicating search string as uppercase
	if (options.ignore_case) {
		search_string = strdup(options.search_string);
		for (index = 0; index < search_string_length; index++) {
			search_string[index] = toupper(search_string[index]);
		}
	}

	// variables for checking whole word matches
	int is_prefix_alphabetic = 0;
	int is_ending_alphabetic = 0;
	if (search_string_length > 0) {
		is_ending_alphabetic = isalpha(search_string[search_string_length - 1]);
	}

	// storing matching substring of the same size as the search string
	char* matching_substring = strdup(options.search_string);
	size_t match_index = 0;

	//3. fgetc(), toupper() returns int instead of char
	int c;
	while ((c = fgetc(file)) != EOF) {
		int c_toupper = options.ignore_case ? toupper(c) : c;
		int c_is_alphabetic = isalpha(c);

		if (match_index == search_string_length) {
			int is_matching = 1;
			if (options.match_whole_words) {
				if (is_prefix_alphabetic || c_is_alphabetic) {
					is_matching = 0;
				}
			}
			if (is_matching) { printf(ANSI_COLOR_RED); }
			printf("%s", matching_substring);
			if (is_matching) { printf(ANSI_COLOR_RESET); }
			is_prefix_alphabetic = is_ending_alphabetic;
			match_index = 0;
		}

		if ((char)c_toupper == search_string[match_index]) {
			matching_substring[match_index] = (char)c;
			match_index += 1;
		} else {
			for (index = 0; index < match_index; index++) {
				printf("%c", matching_substring[index]);
			}
			printf("%c", c);
			is_prefix_alphabetic = c_is_alphabetic;
			match_index = 0;
		}
	}

	// 4. printing matching leftovers at the end of file
	for (index = 0; index < match_index; index++) {
		printf("%c", matching_substring[index]);
	}

	return EXIT_SUCCESS;
}

int main(int argc, char **argv) {
	struct Options options;

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
	// it stores index of the first non-option argument
	if (optind + 1 < argc) {
		options.search_string = argv[optind + 0];
		options.file_name = argv[optind + 1];
	} else {
		printf("Error: Bad arguments.\n");
		return EXIT_FAILURE;
	}

	//3. Printing options.
	printf("Searching in: %s\n", options.file_name);
	printf("Searching for: %s\n", options.search_string);
	printf("Searching ignore case: %s\n", options.ignore_case ? "TRUE" : "FALSE");
	printf("Searching matching only whole words: %s\n", options.match_whole_words ? "TRUE" : "FALSE");

	return grep(options);
}
