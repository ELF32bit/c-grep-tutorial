#include "grep.h"

#include <stdlib.h> // EXIT_SUCCESS, EXIT_FAILURE
#include <stdio.h> // asprintf(), fopen(), getline()
#include <string.h> // strlen(), strdup()
#include <ctype.h> // toupper(), isalpha()

/* asprintf() macro for repeated usage without memory leaks */
/* First use of this macro requires a heap allocated string */
#define ASPRINTF(destination_string,  ...) {\
	char* previous_string = destination_string;\
	asprintf(&(destination_string), __VA_ARGS__);\
	free(previous_string);\
}

GrepLineResult grep_line(const char* line, const GrepOptions* options) {
	/* This time also returning how many matches occured */
	GrepLineResult grep_line_result;
	grep_line_result.colored_line = NULL;
	grep_line_result.match_count = 0;
	grep_line_result.exit_code = EXIT_SUCCESS;

	size_t search_string_length = strlen(options->search_string);
	char* matching_substring = strdup(options->search_string);
	if (matching_substring == NULL) {
		grep_line_result.exit_code = EXIT_FAILURE; // setting error code
		return grep_line_result;
	}
	size_t match_index = 0;

	char* search_string = options->search_string;
	if (options->ignore_case) {
		search_string = strdup(options->search_string);
		if (search_string == NULL) {
			free(matching_substring);
			grep_line_result.exit_code = EXIT_FAILURE; // setting error code
			return grep_line_result;
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

	/* C strings usually end with '\0' terminator */
	/* Consuming line character by character until '\0' terminator */
	char* colored_line = strdup(""); // so free() works
	bool is_colored_line = 0;
	size_t match_count = 0;
	size_t line_index = 0;

	do {
		char c = line[line_index];
		char c_toupper = (char)(options->ignore_case ? toupper(c) : c);
		bool c_is_alpha = isalpha(c);

		/* asprintf() is a very convenient way to create strings */
		/* Each call to asprintf() will allocate a new string */
		/* Special macro is used to free previous strings */

		if (match_index == search_string_length) {
			bool is_matching = !(is_before_alpha && is_prefix_alpha);
			is_matching = is_matching && !(is_suffix_alpha && c_is_alpha);
			is_matching = options->match_whole_words ? is_matching : 1;
			if (is_matching) { is_colored_line = 1; match_count++; }

			if (is_matching) { ASPRINTF(colored_line, "%s%s", colored_line, ANSI_COLOR_RED); }
			ASPRINTF(colored_line, "%s%s", colored_line, matching_substring);
			if (is_matching) { ASPRINTF(colored_line, "%s%s", colored_line, ANSI_COLOR_RESET); }

			is_before_alpha = is_suffix_alpha;
			match_index = 0;
		}

		if (c_toupper == search_string[match_index] && c != '\0') {
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

	/* Freeing colored line if no matches were found */
	if (!is_colored_line) {
		free(colored_line);
		colored_line = NULL;
	}

	if (options->ignore_case) { free(search_string); }
	free(matching_substring);

	/* Preparing to return grep line result */
	grep_line_result.colored_line = colored_line;
	grep_line_result.match_count = match_count;
	return grep_line_result;
}

GrepFileResult grep_file(const char* file_name, const GrepOptions* options) {
	/* This time also returning how many matches occured */
	GrepFileResult grep_file_result;
	grep_file_result.match_count = 0;
	grep_file_result.exit_code = EXIT_SUCCESS;

	FILE *file = fopen(file_name, "r");
	if (file == NULL) {
		grep_file_result.exit_code = EXIT_FAILURE; // setting error code
		return grep_file_result;
	}

	/* Reading file line by line using getline() from stdio.h */
	char* line = NULL;
	size_t line_size = 0; // size of the line buffer
	ssize_t line_length; //extra s means signed, size_t is unsigned by default
	size_t line_number = 0;

	while ((line_length = getline(&line, &line_size, file)) != -1) {
		line_number += 1;
		GrepLineResult grep_line_result = grep_line(line, options);
		grep_file_result.match_count += grep_line_result.match_count;
		if (grep_line_result.colored_line != NULL) {
			printf("%s", ANSI_COLOR_GREEN);
			printf("%zu", line_number);
			printf("%s:%s", ANSI_COLOR_CYAN, ANSI_COLOR_RESET);
			printf("%s", grep_line_result.colored_line);
			free(grep_line_result.colored_line); // free() is necessary here
		}
	}
	if (line != NULL) { free(line); }

	return grep_file_result;
}
