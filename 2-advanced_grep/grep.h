/* All header files need to be protected with preprocessor guards */
#ifndef GREP_H
#define GREP_H

typedef int bool;

/* Always prefix structs with header name */
typedef struct GrepOptions {
	bool ignore_case;
	bool match_whole_words;
	char* search_string;
} GrepOptions;

/* Always prefix functions with header name */
char* grep_line(const char* line, const GrepOptions* options);
int grep_file(const char* file_name, const GrepOptions* options);

#endif
