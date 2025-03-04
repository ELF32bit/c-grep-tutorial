#include "grep.h"

#include <stdlib.h> // EXIT_SUCCESS, EXIT_FAILURE
#include <string.h> // strlen(), strdup()
#include <ctype.h> // toupper(), isalpha()
#include <stdio.h> // asprintf()

/* asprintf() macro for repeated usage without memory leaks */
/* First use of this macro requires a heap allocated string */
#define ASPRINTF(destination_string,  ...) {\
	char* previous_string = destination_string;\
	asprintf(&(destination_string), __VA_ARGS__);\
	free(previous_string);\
}

GrepStringResult grep_string(const char* string, const GrepOptions* options) {
	GrepStringResult grep_string_result;
	grep_string_result.colored_string = NULL;
	grep_string_result.match_count = 0;
	grep_string_result.exit_code = EXIT_SUCCESS;

	size_t search_string_length = strlen(options->search_string);
	char* matching_substring = strdup(options->search_string);
	if (matching_substring == NULL) {
		grep_string_result.exit_code = EXIT_FAILURE;
		return grep_string_result;
	}
	size_t match_index = 0;

	char* search_string = options->search_string;
	if (options->ignore_case) {
		search_string = strdup(options->search_string);
		if (search_string == NULL) {
			free(matching_substring);
			grep_string_result.exit_code = EXIT_FAILURE;
			return grep_string_result;
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

	char* colored_string = strdup("");
	bool is_colored_string = 0;
	size_t match_count = 0;
	size_t string_index = 0;

	char c;
	do {
		c = string[string_index]; string_index++;
		char c_toupper = (char)(options->ignore_case ? toupper(c) : c);
		bool c_is_alpha = isalpha(c);

		if (match_index == search_string_length) {
			bool is_matching = !(is_before_alpha && is_prefix_alpha);
			is_matching = is_matching && !(is_suffix_alpha && c_is_alpha);
			is_matching = options->match_whole_words ? is_matching : 1;
			if (is_matching) { is_colored_string = 1; match_count++; }

			if (is_matching) { ASPRINTF(colored_string, "%s%s", colored_string, ANSI_COLOR_RED); }
			ASPRINTF(colored_string, "%s%s", colored_string, matching_substring);
			if (is_matching) { ASPRINTF(colored_string, "%s%s", colored_string, ANSI_COLOR_RESET); }

			is_before_alpha = is_suffix_alpha;
			match_index = 0;
		}

		if (c_toupper == search_string[match_index] && c != '\0') {
			matching_substring[match_index] = c;
			match_index += 1;
		} else {
			for (size_t index = 0; index < match_index; index++) {
				ASPRINTF(colored_string, "%s%c", colored_string, matching_substring[index]);
			}
			if (c != '\0') { ASPRINTF(colored_string, "%s%c", colored_string, c); }

			is_before_alpha = c_is_alpha;
			match_index = 0;
		}
	} while (c != '\0');

	if (!is_colored_string) {
		free(colored_string);
		colored_string = NULL;
	}

	if (options->ignore_case) { free(search_string); }
	free(matching_substring);

	grep_string_result.colored_string = colored_string;
	grep_string_result.match_count = match_count;
	return grep_string_result;
}
