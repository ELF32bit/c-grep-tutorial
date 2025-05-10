#include "grep.h"

#include <wchar.h> // wcslen(), wcsdup()
#include <wctype.h> // towupper(), iswalpha()
#include <stdio.h> // asprintf()

wchar_t* convert_string(const char* string) {
	size_t string_size = strlen(string) + 1;
	wchar_t* wide_character_string = calloc(string_size, sizeof(wchar_t));
	if (wide_character_string == NULL) { return NULL; }
	if (mbstowcs(wide_character_string, string, string_size) == (size_t)-1) {
		free(wide_character_string);
		return NULL;
	}
	return wide_character_string;
}

/* asprintf() macro for repeated usage without memory leaks */
/* First use of this macro requires a heap allocated string */
#define ASPRINTF(destination_string,  ...) {\
	char* previous_string = destination_string;\
	asprintf(&(destination_string), __VA_ARGS__);\
	free(previous_string);\
}

GrepStringResult grep_string(const char* string, const GrepOptions* options) {
	/* This time also returning how many matches occured */
	GrepStringResult grep_string_result;
	grep_string_result.colored_string = NULL;
	grep_string_result.match_count = 0;
	grep_string_result.exit_code = EXIT_SUCCESS;

	size_t search_string_length = wcslen(options->search_string);
	if (search_string_length == 0) {
		grep_string_result.exit_code = EXIT_SUCCESS;
		return grep_string_result;
	}

	/* Converting source string to wide character string */
	wchar_t* source_string = convert_string(string);
	if (source_string == NULL) {
		grep_string_result.exit_code = EXIT_FAILURE;
		return grep_string_result;
	}

	wchar_t* matching_substring = wcsdup(options->search_string);
	if (matching_substring == NULL) {
		free(source_string);
		grep_string_result.exit_code = EXIT_FAILURE; // setting error code
		return grep_string_result;
	}
	size_t match_index = 0;

	wchar_t* search_string = options->search_string;
	if (options->ignore_case) {
		search_string = wcsdup(options->search_string);
		if (search_string == NULL) {
			free(matching_substring); free(source_string);
			grep_string_result.exit_code = EXIT_FAILURE;
			return grep_string_result;
		}
		for (size_t index = 0; index < search_string_length; index++) {
			search_string[index] = (wchar_t)towupper(search_string[index]);
		}
	}

	bool is_before_alpha = 0;
	bool is_prefix_alpha = iswalpha(search_string[0]);;
	bool is_suffix_alpha = iswalpha(search_string[search_string_length - 1]);

	/* C strings usually end with '\0' terminator */
	/* Consuming string character by character until '\0' terminator */
	bool is_colored_string = 0;
	char* colored_string = strdup(""); // so free() works
	if (colored_string == NULL) {
		if (options->ignore_case) { free(search_string); }
		free(matching_substring); free(source_string);
		grep_string_result.exit_code = EXIT_FAILURE; // setting error code
		return grep_string_result;
	}
	size_t source_string_index = 0;
	size_t match_count = 0;

	wchar_t c;
	do {
		c = source_string[source_string_index]; source_string_index++;
		wchar_t c_toupper = (wchar_t)(options->ignore_case ? towupper(c) : c);
		bool c_is_alpha = iswalpha(c);

		/* asprintf() provides a very convenient way to create strings */
		/* Each call to asprintf() will allocate a new string */
		/* Special macro is used to free previous strings */

		if (match_index > 0 && match_index == search_string_length) {
			bool is_matching = !(is_before_alpha && is_prefix_alpha);
			is_matching = is_matching && !(is_suffix_alpha && c_is_alpha);
			is_matching = options->match_whole_words ? is_matching : 1;
			if (is_matching) { is_colored_string = 1; match_count++; }

			if (is_matching) { ASPRINTF(colored_string, "%s%s", colored_string, ANSI_COLOR_RED); }
			ASPRINTF(colored_string, "%s%ls", colored_string, matching_substring);
			if (is_matching) { ASPRINTF(colored_string, "%s%s", colored_string, ANSI_COLOR_RESET); }

			is_before_alpha = is_suffix_alpha;
			match_index = 0;
		}

		if (c_toupper == search_string[match_index] && c != L'\0') {
			matching_substring[match_index] = c;
			match_index += 1;
		} else {
			for (size_t index = 0; index < match_index; index++) {
				ASPRINTF(colored_string, "%s%lc", colored_string, matching_substring[index]);
			}
			if (c != L'\0') { ASPRINTF(colored_string, "%s%lc", colored_string, c); }

			is_before_alpha = c_is_alpha;
			match_index = 0;
		}
	} while (c != L'\0');

	/* Freeing colored string if no matches were found */
	if (!is_colored_string) {
		free(colored_string);
		colored_string = NULL;
	}

	if (options->ignore_case) { free(search_string); }
	free(matching_substring);
	free(source_string);

	/* Preparing to return grep string result */
	grep_string_result.colored_string = colored_string;
	grep_string_result.match_count = match_count;
	return grep_string_result;
}
