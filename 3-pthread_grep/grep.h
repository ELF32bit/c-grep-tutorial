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
char* grep_line(char* line, struct GrepOptions options);
int grep_file(char* file_name, struct GrepOptions options);
int grep_files(char** file_names, int file_names_length, struct GrepOptions options);

#endif
