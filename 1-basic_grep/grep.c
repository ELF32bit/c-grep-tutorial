#include <stdlib.h> // EXIT_SUCCESS, EXIT_FAILURE
#include <stdio.h> // printf(), fopen()
#include <unistd.h> // getopt()
#include <string.h> // strlen(), strdup()
#include <ctype.h> // toupper(), isalpha()
typedef int bool; // for easier reading

// terminal colors
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_RESET "\x1b[0m"

struct Options {
	bool ignore_case;
	bool match_whole_words;
	char* search_string;
	char* file_name;
};

int grep(const struct Options* options) {
	// 1. trying to open input file for reading
	FILE* file = fopen(options->file_name, "r");
	if (file == NULL) {
		printf("Error: No such file.\n");
		return EXIT_FAILURE;
	}

	//2. preparing variables
	// size_t stores max array size on x64 or x86 system
	size_t search_string_length = strlen(options->search_string);
	// creating matching substring of the same size as the search string
	char* matching_substring = strdup(options->search_string);
	if (matching_substring == NULL) { return EXIT_FAILURE; }
	size_t match_index = 0;

	// duplicating search string as upper case if necessary
	char* search_string = options->search_string;
	if (options->ignore_case) {
		search_string = strdup(options->search_string);
		if (search_string == NULL) { return EXIT_FAILURE; }
		for (size_t index = 0; index < search_string_length; index++) {
			search_string[index] = toupper(search_string[index]);
		}
	}

	// variables for checking alphabetic whole word matches
	bool is_before_match_alphabetic = 0;
	bool is_search_string_end_alphabetic = 0;
	if (search_string_length > 0) {
		char search_string_end = search_string[search_string_length - 1];
		is_search_string_end_alphabetic = isalpha(search_string_end);
	}

	//3. consuming file character by character until 'EOF'
	int c;
	do {
		c = fgetc(file); // fgetc() returns int instead of char
		int c_toupper = options->ignore_case ? toupper(c) : c;
		bool c_is_alphabetic = isalpha(c);

		// processing full matching substring
		if (match_index == search_string_length) {
			bool is_matching = 1;
			if (options->match_whole_words) {
				if (is_before_match_alphabetic || c_is_alphabetic) {
					is_matching = 0;
				}
			}
			if (is_matching) { printf(ANSI_COLOR_RED); }
			printf("%s", matching_substring);
			if (is_matching) { printf(ANSI_COLOR_RESET); }
			is_before_match_alphabetic = is_search_string_end_alphabetic;
			match_index = 0;
		}

		// growing matching substring
		if ( ((char)c_toupper == search_string[match_index]) && (c != EOF) ) {
			matching_substring[match_index] = (char)c;
			match_index += 1;
		} else {
			for (size_t index = 0; index < match_index; index++) {
				printf("%c", matching_substring[index]);
			}
			if (c != EOF) { printf("%c", c); }
			is_before_match_alphabetic = c_is_alphabetic;
			match_index = 0;
		}
	} while (c != EOF);

	//4. freeing memory used by duplicated strings
	if (options->ignore_case) { free(search_string); }
	free(matching_substring);

	return EXIT_SUCCESS;
}

int main(int argc, char **argv) {
	struct Options options;
	options.ignore_case = 0;
	options.match_whole_words = 0;
	options.search_string = NULL;
	options.file_name = NULL;

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
		options.file_name = argv[optind + 1];
	} else {
		printf("Error: Bad arguments.\n");
		return EXIT_FAILURE;
	}

	//3. printing options
	printf("Searching in: %s\n", options.file_name);
	printf("Searching for: %s\n", options.search_string);
	printf("Ignoring case: %s\n", options.ignore_case ? "TRUE" : "FALSE");
	printf("Matching whole words: %s\n", options.match_whole_words ? "TRUE" : "FALSE");

	return grep(&options); // options are passed as read only reference without copy
}
