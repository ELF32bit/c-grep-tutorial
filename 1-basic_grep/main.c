#include <stdlib.h> // EXIT_SUCCESS, EXIT_FAILURE
#include <stdio.h> // printf(), fopen(), fclose()
#include <unistd.h> // getopt()
#include <string.h> // strlen(), strdup()
#include <ctype.h> // toupper(), isalpha()

/* Terminal color codes */
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_RESET "\x1b[0m"

/* Use typedef to distinguish variables */
typedef int bool;

struct Options {
	bool ignore_case;
	bool match_whole_words;
	char* search_string;
	char* file_name;
};

/* Options are passed as read only reference instead of copy */
int grep(const struct Options* options) {
	/* Trying to open input file for reading */
	FILE* file = fopen(options->file_name, "r");
	if (file == NULL) {
		printf("Error: No such file.\n");
		return EXIT_FAILURE;
	}

	/* Allocating a string of the same length as the search string */
	size_t search_string_length = strlen(options->search_string);
	char* matching_substring = strdup(options->search_string);
	if (matching_substring == NULL) { return EXIT_FAILURE; }
	size_t match_index = 0;

	/* Duplicating search string in upper case if necessary */
	char* search_string = options->search_string;
	if (options->ignore_case) {
		search_string = strdup(options->search_string);
		if (search_string == NULL) {
			free(matching_substring);
			return EXIT_FAILURE;
		}
		/* Always use size_t index to iterate over generic arrays */
		for (size_t index = 0; index < search_string_length; index++) {
			search_string[index] = toupper(search_string[index]);
		}
	}

	/* Variables required for detecting alphabetic whole word matches */
	bool is_before_alpha = 0;
	bool is_prefix_alpha = 0;
	bool is_suffix_alpha = 0;
	if (search_string_length > 0) {
		is_prefix_alpha = isalpha(search_string[0]);
		is_suffix_alpha = isalpha(search_string[search_string_length - 1]);
	}

	/* Consuming file character by character until 'EOF' */
	int c;
	do {
		c = fgetc(file); // fgetc() returns int instead of char
		char c_toupper = (char)(options->ignore_case ? toupper(c) : c);
		bool c_is_alpha = isalpha(c);

		/* Matching substring is detected */
		if (match_index == search_string_length) {
			bool is_matching = !(is_before_alpha && is_prefix_alpha);
			is_matching = is_matching && !(is_suffix_alpha && c_is_alpha);
			is_matching = options->match_whole_words ? is_matching : 1;

			if (is_matching) { printf(ANSI_COLOR_RED); }
			printf("%s", matching_substring);
			if (is_matching) { printf(ANSI_COLOR_RESET); }

			is_before_alpha = is_suffix_alpha;
			match_index = 0;
		}

		/* Growing matching substring */
		if (c_toupper == search_string[match_index] && c != EOF) {
			matching_substring[match_index] = (char)c;
			match_index += 1;
		} else {
			/* Printing incomplete match */
			for (size_t index = 0; index < match_index; index++) {
				printf("%c", matching_substring[index]);
			}
			if (c != EOF) { printf("%c", c); }

			is_before_alpha = c_is_alpha;
			match_index = 0;
		}
	} while (c != EOF);

	/* Freeing memory allocated by helper strings and closing file */
	if (options->ignore_case) { free(search_string); }
	free(matching_substring);
	fclose(file);

	return EXIT_SUCCESS;
}

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

	/* Passing structs by reference is usually cheaper */
	return grep(&options);
}
