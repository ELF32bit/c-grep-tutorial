#include "grep.h"

#include <stdlib.h> // EXIT_SUCCESS, EXIT_FAILURE
#include <stdio.h> // asprintf(), fopen(), getline()
#include <string.h> // strlen(), strdup()
#include <ctype.h> // toupper(), isalpha()

#include <pthread.h>

/* Terminal color codes */
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_RESET "\x1b[0m"

/* asprintf() macro for repeated usage without memory leaks */
/* First use of this macro requires a heap allocated string */
#define ASPRINTF(destination_string,  ...) {\
	char* previous_string = destination_string;\
	asprintf(&(destination_string), __VA_ARGS__);\
	free(previous_string);\
}

void grep_result_free(GrepResult* grep_result) {
	if (grep_result != NULL) {
		if (grep_result->colored_string != NULL) {
			free(grep_result->colored_string);
		}
		free(grep_result);
	}
	grep_result = NULL;
}

GrepResult* grep_line(const char* line, const GrepOptions* options) {
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

	char* colored_line = strdup("");
	bool is_colored_line = 0;
	size_t match_count = 0;
	size_t line_index = 0;

	do {
		char c = line[line_index];
		char c_toupper = (char)(options->ignore_case ? toupper(c) : c);
		bool c_is_alpha = isalpha(c);

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

	if (!is_colored_line) {
		free(colored_line);
		colored_line = NULL;
	}

	if (options->ignore_case) { free(search_string); }
	free(matching_substring);

	GrepResult* grep_result = malloc(sizeof(GrepResult));
	grep_result->colored_string = colored_line;
	grep_result->match_count = match_count;
	return grep_result;
}

GrepResult* grep_file(const char* file_name, const GrepOptions* options) {
	FILE *file = fopen(file_name, "r");
	if (file == NULL) {
		printf("Error: No such file.\n");
		return NULL;
	}

	GrepResult* grep_result = malloc(sizeof(GrepResult));
	grep_result->colored_string = NULL; // not necessary for files
	grep_result->match_count = 0;

	char* line = NULL;
	size_t line_size = 0;
	ssize_t line_length;
	size_t line_number = 0;

	while ((line_length = getline(&line, &line_size, file)) != -1) {
		line_number += 1;
		GrepResult* line_grep_result = grep_line(line, options);
		if (line_grep_result->colored_string != NULL) {
			printf("[%s] %zu: %s", file_name, line_number, line_grep_result->colored_string);
		}
		grep_result->match_count += line_grep_result->match_count;
		grep_result_free(line_grep_result);
	}
	if (line != NULL) { free(line); }

	return grep_result;
}
