#include "grep.h"

#include <stdlib.h> // EXIT_SUCCESS, EXIT_FAILURE
#include <stdio.h> // asprintf(), fopen(), getline()
#include <string.h> // strlen(), strdup()
#include <ctype.h> // toupper(), isalpha()

// terminal colors
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_RESET "\x1b[0m"

// asprintf() macro for repeated usage without memory leaks
#define ASPRINTF(destination_string,  ...) {\
	char* previous_string = destination_string;\
	asprintf(&(destination_string), __VA_ARGS__);\
	free(previous_string);\
}

char* grep_line(const char* line, const GrepOptions* options) {
	size_t search_string_length = strlen(options->search_string);
	char* matching_substring = strdup(options->search_string);
	if (matching_substring == NULL) { return NULL; }
	size_t match_index = 0;

	char* search_string = options->search_string;
	if (options->ignore_case) {
		search_string = strdup(options->search_string);
		if (search_string == NULL) {
			free(matching_substring);
			return NULL;
		}
		for (size_t index = 0; index < search_string_length; index++) {
			search_string[index] = toupper(search_string[index]);
		}
	}

	bool is_before_alpha = 0;
	bool is_prefix_alpha = 0;
	bool is_suffix_alpha = 0;
	if (search_string_length > 0) {
		is_prefix_alpha = isalpha(search_string[0]);
		is_suffix_alpha = isalpha(search_string[search_string_length - 1]);
	}

	//1. consuming line character by character until '\0' terminator
	// asprintf() macro is used to append to the colored line
	char* colored_line = strdup(""); // so free() works
	bool is_colored_line = 0;
	size_t line_index = 0;

	do {
		char c = line[line_index];
		char c_toupper = (char)(options->ignore_case ? toupper(c) : c);
		bool c_is_alpha = isalpha(c);

		if (match_index == search_string_length) {
			bool is_matching = !(is_before_alpha && is_prefix_alpha);
			is_matching = is_matching && !(is_suffix_alpha && c_is_alpha);
			is_matching = options->match_whole_words ? is_matching : 1;
			if (is_matching) { is_colored_line = 1; }

			if (is_matching) { ASPRINTF(colored_line, "%s%s", colored_line, ANSI_COLOR_RED); }
			ASPRINTF(colored_line, "%s%s", colored_line, matching_substring);
			if (is_matching) { ASPRINTF(colored_line, "%s%s", colored_line, ANSI_COLOR_RESET); }

			is_before_alpha = is_suffix_alpha;
			match_index = 0;
		}

		if ( (c_toupper == search_string[match_index]) && (c != '\0') ) {
			matching_substring[match_index] = c;
			match_index += 1;
		} else {
			for (size_t index = 0; index < match_index; index++) {
				ASPRINTF(colored_line, "%s%c", colored_line, matching_substring[index]);
			}
			if (c != '\0') { ASPRINTF(colored_line, "%s%c", colored_line, c); }

			is_before_alpha = c_is_alpha;
			match_index = 0;
		}

		line_index++;
	} while (line[line_index] != '\0');

	//2. freeing colored line if no matches were found
	if (!is_colored_line) {
		free(colored_line);
		colored_line = NULL;
	}

	if (options->ignore_case) { free(search_string); }
	free(matching_substring);

	return colored_line;
}

int grep_file(const char* file_name, const GrepOptions* options) {
	FILE *file = fopen(file_name, "r");
	if (file == NULL) {
		printf("Error: No such file.\n");
		return EXIT_FAILURE;
	}

	//1. reading file line by line using getline() from stdio.h
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
