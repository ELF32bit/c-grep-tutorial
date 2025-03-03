/* All header files need to be protected with preprocessor guards */
#ifndef GREP_H
#define GREP_H

#include <stddef.h> // size_t

typedef int bool;

/* Always prefix structs with header name */
typedef struct GrepOptions {
	bool ignore_case;
	bool match_whole_words;
	char* search_string;
} GrepOptions;

/* Use typedef for structs to improve readability */
typedef struct GrepResult {
	char* colored_string;
	size_t match_count;
} GrepResult;

/* Always prefix functions with header name */
GrepResult* grep_line(const char* line, const GrepOptions* options);
GrepResult* grep_file(const char* file_name, const GrepOptions* options);
int grep_files(char** file_names, int file_names_length, const GrepOptions* options);

void grep_result_free(GrepResult* grep_result);

#endif
