// all header files need to be protected with preprocessor guards
#ifndef GREP_H
#define GREP_H

typedef int bool;

// always prefix structs with header name
struct GrepOptions {
	bool ignore_case;
	bool match_whole_words;
	char* search_string;
};

// always prefix functions with header name
char* grep_line(const char* line, const struct GrepOptions* options);
int grep_file(const char* file_name, const struct GrepOptions* options);

#endif
