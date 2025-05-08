#include "grep.h"

#include <stdio.h> // fopen(), getline(), printf(), fclose()

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
		GrepStringResult grep_string_result = grep_string(line, options);
		grep_file_result.match_count += grep_string_result.match_count;
		if (grep_string_result.colored_string != NULL) {
			printf("%s", ANSI_COLOR_GREEN);
			printf("%zu", line_number);
			printf("%s:%s", ANSI_COLOR_CYAN, ANSI_COLOR_RESET);
			printf("%s", grep_string_result.colored_string);
			free(grep_string_result.colored_string); // free() after use
		}
	}

	if (line != NULL) { free(line); }
	fclose(file);

	return grep_file_result;
}
