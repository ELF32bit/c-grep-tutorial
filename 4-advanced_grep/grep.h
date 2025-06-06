/* All header files need to be protected with preprocessor guards */
#ifndef GREP_H
#define GREP_H

/* Terminal color codes */
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_RESET "\x1b[0m"

#include <stddef.h> // size_t

typedef int bool;

/* Always prefix structs with header name */
typedef struct GrepOptions {
	bool ignore_case;
	bool match_whole_words;
	wchar_t* search_string;
} GrepOptions;

/* Use typedef for structs to improve readability */
typedef struct GrepStringResult {
	char* colored_string; // free() after use
	size_t match_count;
	int exit_code;
} GrepStringResult;

typedef struct GrepFileResult {
	size_t match_count;
	int exit_code;
} GrepFileResult;

wchar_t* convert_string(const char* string);

/* Always prefix functions with header name */
GrepStringResult grep_string(const char* string, const GrepOptions* options);
GrepFileResult grep_file(const char* file_name, const GrepOptions* options);

#endif
