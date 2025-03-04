#include "grep.h"

#include <stdlib.h> // EXIT_SUCCESS, EXIT_FAILURE
#include <stdio.h> // fopen(), getline(), printf()

GrepFileResult grep_file(const char* file_name, const GrepOptions* options) {
	GrepFileResult grep_file_result;
	grep_file_result.match_count = 0;
	grep_file_result.exit_code = EXIT_SUCCESS;

	FILE *file = fopen(file_name, "r");
	if (file == NULL) {
		grep_file_result.exit_code = EXIT_FAILURE;
		return grep_file_result;
	}

	char* line = NULL;
	size_t line_size = 0;
	ssize_t line_length;
	size_t line_number = 0;

	while ((line_length = getline(&line, &line_size, file)) != -1) {
		line_number += 1;
		GrepStringResult grep_string_result = grep_string(line, options);
		grep_file_result.match_count += grep_string_result.match_count;
		if (grep_string_result.colored_string != NULL) {
			if (!options->_quiet) { // checking hidden internal option
				printf("%s", ANSI_COLOR_GREEN);
				printf("%zu", line_number);
				printf("%s:%s", ANSI_COLOR_CYAN, ANSI_COLOR_RESET);
				printf("%s", grep_string_result.colored_string);
			}
			free(grep_string_result.colored_string); // free() after use
		}
	}

	if (line != NULL) { free(line); }
	fclose(file);

	return grep_file_result;
}
