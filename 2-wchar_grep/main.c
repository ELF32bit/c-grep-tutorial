#include <stdlib.h> // EXIT_SUCCESS, EXIT_FAILURE, calloc(), mbstowcs()
#include <stdio.h> // printf(), fopen(), fclose()
#include <unistd.h> // getopt()
#include <string.h> // strlen()

/* Using same functions designed for wide characters */
#include <locale.h> // setlocale()
#include <wchar.h> // wcslen(), wcsdup()
#include <wctype.h> // towupper(), iswalpha()

/* Terminal color codes */
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_RESET "\x1b[0m"

/* Use typedef to distinguish variables */
typedef int bool;

struct Options {
	bool ignore_case;
	bool match_whole_words;
	wchar_t* search_string; // free() after use
	char* file_name;
};

int grep(const struct Options* options) {
	FILE* file = fopen(options->file_name, "r");
	if (file == NULL) {
		printf("Error: No such file.\n");
		return EXIT_FAILURE;
	}

	size_t search_string_length = wcslen(options->search_string);
	wchar_t* matching_substring = wcsdup(options->search_string);
	if (matching_substring == NULL) { return EXIT_FAILURE; }
	size_t match_index = 0;

	wchar_t* search_string = options->search_string;
	if (options->ignore_case) {
		search_string = wcsdup(options->search_string);
		if (search_string == NULL) {
			free(matching_substring);
			return EXIT_FAILURE;
		}
		for (size_t index = 0; index < search_string_length; index++) {
			search_string[index] = towupper(search_string[index]);
		}
	}

	bool is_before_alpha = 0;
	bool is_prefix_alpha = 0;
	bool is_suffix_alpha = 0;
	if (search_string_length > 0) {
		is_prefix_alpha = iswalpha(search_string[0]);
		is_suffix_alpha = iswalpha(search_string[search_string_length - 1]);
	}

	/* Consuming file character by character until 'WEOF' */
	wint_t c;
	do {
		c = fgetwc(file); // fgetwc() returns wint_t instead of wchar_t
		wchar_t c_toupper = (wchar_t)(options->ignore_case ? towupper(c) : c);
		bool c_is_alpha = iswalpha(c);

		if (match_index == search_string_length) {
			bool is_matching = !(is_before_alpha && is_prefix_alpha);
			is_matching = is_matching && !(is_suffix_alpha && c_is_alpha);
			is_matching = options->match_whole_words ? is_matching : 1;

			if (is_matching) { printf(ANSI_COLOR_RED); }
			printf("%ls", matching_substring);
			if (is_matching) { printf(ANSI_COLOR_RESET); }

			is_before_alpha = is_suffix_alpha;
			match_index = 0;
		}

		if (c_toupper == search_string[match_index] && c != WEOF) {
			matching_substring[match_index] = (wchar_t)c;
			match_index += 1;
		} else {
			for (size_t index = 0; index < match_index; index++) {
				printf("%lc", matching_substring[index]);
			}
			if (c != WEOF) { printf("%lc", c); }

			is_before_alpha = c_is_alpha;
			match_index = 0;
		}
	} while (c != WEOF);

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

	/* setlocale() modifies the behavior of locale-dependent functions */
	/* mbstowcs(), towupper(), iswalpha() are affected */
	setlocale(LC_ALL, "C.UTF8");

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

	if (optind + 1 < argc) {
		char* search_string = argv[optind + 0];
		size_t search_string_size = strlen(search_string) + 1;

		/* Allocating terminated wide character string of enough size */
		wchar_t* wcs = calloc(search_string_size, sizeof(wchar_t));
		if (wcs == NULL) { return EXIT_FAILURE; } // not enough memory
		if (mbstowcs(wcs, search_string, search_string_size) == (size_t)-1) {
			free(wcs);
			printf("Error: Failed converting search string.\n");
			return EXIT_FAILURE;
		}

		options.search_string = wcs;
		options.file_name = argv[optind + 1];
	} else {
		printf("Error: Bad arguments.\n");
		return EXIT_FAILURE;
	}

	/* This time it's necessary to hold exit code before returning */
	int grep_result = grep(&options);

	free(options.search_string); // free() after use
	return grep_result;
}
