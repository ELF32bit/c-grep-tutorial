#include "grep.h"

#include <stdlib.h> // EXIT_SUCCESS, EXIT_FAILURE
#include <stdio.h> // asprintf(), fopen(), getline()
#include <string.h> // strlen(), strdup()
#include <ctype.h> // toupper(), isalpha()

// terminal colors
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_RESET "\x1b[0m"

char* grep_line(char* line, struct GrepOptions options) {
	//1. preparing variables
	// size_t stores max array size on x64 or x86 system
	size_t search_string_length = strlen(options.search_string);
	// creating matching substring of the same size as the search string
	char* matching_substring = strdup(options.search_string);
	if (matching_substring == NULL) { return NULL; }
	size_t match_index = 0;

	// duplicating search string as upper case if necessary
	char* search_string = options.search_string;
	if (options.ignore_case) {
		search_string = strdup(options.search_string);
		if (search_string == NULL) { return NULL; }
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

	//2. consuming line character by character until '\0' terminator
	// asprintf() is first used here to create the colored line
	char* colored_line = "";
	bool is_colored_line = 0;
	size_t line_index = 0;

	do {
		char c = line[line_index];
		char c_toupper = (char)(options.ignore_case ? toupper(c) : c);
		bool c_is_alphabetic = isalpha(c);

		// processing full matching substring
		if (match_index == search_string_length) {
			bool is_matching = 1;
			if (options.match_whole_words) {
				if (is_before_match_alphabetic || c_is_alphabetic) {
					is_matching = 0;
				}
			}
			if (is_matching) {
				is_colored_line = 1;
				asprintf(&colored_line, "%s%s", colored_line, ANSI_COLOR_RED);
			}
			asprintf(&colored_line, "%s%s", colored_line, matching_substring);
			if (is_matching) { asprintf(&colored_line, "%s%s", colored_line, ANSI_COLOR_RESET); }
			is_before_match_alphabetic = is_search_string_end_alphabetic;
			match_index = 0;
		}

		// growing matching substring
		if ( (c_toupper == search_string[match_index]) && (c != '\0') ) {
			matching_substring[match_index] = c;
			match_index += 1;
		} else {
			for (size_t index = 0; index < match_index; index++) {
				asprintf(&colored_line, "%s%c", colored_line, matching_substring[index]);
			}
			if (c != '\0') { asprintf(&colored_line, "%s%c", colored_line, c); }
			is_before_match_alphabetic = c_is_alphabetic;
			match_index = 0;
		}
		line_index++;
	} while (line[line_index] != '\0');

	// freeing colored line if no matches were found
	if (!is_colored_line) {
		free(colored_line);
		colored_line = NULL;
	}

	//4. freeing memory used by duplicated strings
	if (options.ignore_case) { free(search_string); }
	free(matching_substring);

	return colored_line;
}

int grep_file(char* file_name, struct GrepOptions options) {
	// 1. trying to open input file for reading
	FILE *file = fopen(file_name, "r");
	if (file == NULL) {
		printf("Error: No such file.\n");
		return EXIT_FAILURE;
	}

	//2. getline() is a newer and better function defined in stdio.h
	char* line = NULL;
	size_t line_size = 0; // size of the line buffer
	ssize_t line_length; //extra s means signed, size_t is unsigned by default
	size_t line_number = 0;

	while ((line_length = getline(&line, &line_size, file)) != -1) {
		line_number += 1;
		char* colored_line = grep_line(line, options);
		if (colored_line != NULL) {
			printf("%zu: %s", line_number, colored_line);
			free(colored_line);
		}
	}
	if (line != NULL) { free(line); }

	return EXIT_SUCCESS;
}
