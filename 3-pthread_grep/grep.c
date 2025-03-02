#include "grep.h"

#include <stdlib.h> // EXIT_SUCCESS, EXIT_FAILURE
#include <stdio.h> // asprintf(), fopen(), getline()
#include <string.h> // strlen(), strdup()
#include <ctype.h> // toupper(), isalpha()

#include <pthread.h>

// terminal colors
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_RESET "\x1b[0m"

// asprintf() macro for repeated usage without memory leaks
#define M_ASPRINTF(destination_string,  ...) {\
	char* previous_string = destination_string;\
	asprintf(&(destination_string), __VA_ARGS__);\
	free(previous_string);\
}

char* grep_line(char* line, struct GrepOptions options) {
	//1. preparing variables
	// size_t stores max array size on x64 or x86 system
	size_t search_string_length = strlen(options.search_string);
	// creating matching substring of the same size as the search string
	char* matching_substring = strdup(options.search_string);
	if (matching_substring == NULL) { return NULL; }
	size_t match_index = 0;

	// duplicating search string as upper case if necessary
	char* search_string = options.search_string;
	if (options.ignore_case) {
		search_string = strdup(options.search_string);
		if (search_string == NULL) { return NULL; }
		for (size_t index = 0; index < search_string_length; index++) {
			search_string[index] = toupper(search_string[index]);
		}
	}

	// variables for checking alphabetic whole word matches
	bool is_before_match_alphabetic = 0;
	bool is_search_string_end_alphabetic = 0;
	if (search_string_length > 0) {
		char search_string_end = search_string[search_string_length - 1];
		is_search_string_end_alphabetic = isalpha(search_string_end);
	}

	//2. consuming line character by character until '\0' terminator
	// asprintf() macro is used to dynamically extend colored line
	char* colored_line = strdup(""); // so free() works
	bool is_colored_line = 0;
	size_t line_index = 0;

	do {
		char c = line[line_index];
		char c_toupper = (char)(options.ignore_case ? toupper(c) : c);
		bool c_is_alphabetic = isalpha(c);

		// processing full matching substring
		if (match_index == search_string_length) {
			bool is_matching = 1;
			if (options.match_whole_words) {
				if (is_before_match_alphabetic || c_is_alphabetic) {
					is_matching = 0;
				}
			}
			if (is_matching) {
				is_colored_line = 1;
				M_ASPRINTF(colored_line, "%s%s", colored_line, ANSI_COLOR_RED);
			}
			M_ASPRINTF(colored_line, "%s%s", colored_line, matching_substring);
			if (is_matching) { M_ASPRINTF(colored_line, "%s%s", colored_line, ANSI_COLOR_RESET); }
			is_before_match_alphabetic = is_search_string_end_alphabetic;
			match_index = 0;
		}

		// growing matching substring
		if ( (c_toupper == search_string[match_index]) && (c != '\0') ) {
			matching_substring[match_index] = c;
			match_index += 1;
		} else {
			for (size_t index = 0; index < match_index; index++) {
				M_ASPRINTF(colored_line, "%s%c", colored_line, matching_substring[index]);
			}
			if (c != '\0') { M_ASPRINTF(colored_line, "%s%c", colored_line, c); }
			is_before_match_alphabetic = c_is_alphabetic;
			match_index = 0;
		}
		line_index++;
	} while (line[line_index] != '\0');

	// freeing colored line if no matches were found
	if (!is_colored_line) {
		free(colored_line);
		colored_line = NULL;
	}

	//4. freeing memory used by duplicated strings
	if (options.ignore_case) { free(search_string); }
	free(matching_substring);

	return colored_line;
}

int grep_file(char* file_name, struct GrepOptions options) {
	// 1. trying to open input file for reading
	FILE *file = fopen(file_name, "r");
	if (file == NULL) {
		printf("Error: No such file.\n");
		return EXIT_FAILURE;
	}

	//2. getline() is a newer and better function defined in stdio.h
	char* line = NULL;
	size_t line_size = 0; // size of the line buffer
	ssize_t line_length; //extra s means signed, size_t is unsigned by default
	size_t line_number = 0;

	while ((line_length = getline(&line, &line_size, file)) != -1) {
		line_number += 1;
		char* colored_line = grep_line(line, options);
		if (colored_line != NULL) {
			printf("[%s] %zu: %s", file_name, line_number, colored_line);
			free(colored_line);
		}
	}
	if (line != NULL) { free(line); }

	return EXIT_SUCCESS;
}

// this structure is not declared in the header file, it's local to this file
struct GrepFileArguments {
	char* file_name;
	struct GrepOptions options;
};

// this function is not declared in the header file, it's local to this file
void *pthread_grep_file(void* arguments) {
	// casting pointer to arguments structure from void* to its proper type
	struct GrepFileArguments* args = (struct GrepFileArguments*) arguments;
	grep_file(args->file_name, args->options);
	return NULL;
}

int grep_files(char** file_names, int file_names_length, struct GrepOptions options) {
	pthread_t thread1, thread2; // using 2 threads for simplicity

	// creating threads mutex
	pthread_mutex_t mutex;
	if (pthread_mutex_init(&mutex, NULL)) {
		printf("Error: Failed to create threads mutex.");
		return EXIT_FAILURE;
	}

	int index = 0;
	int error1, error2;
	while (index < file_names_length) {
		// creating arguments for the first thread
		struct GrepFileArguments arguments1;
		arguments1.file_name = file_names[index];
		arguments1.options = options;
		// starting first thread for files with even index
		error1 = pthread_create(&thread1, NULL, &pthread_grep_file, (void*)&arguments1);
		if (error1) { printf("Error: Failed to create thread1."); };

		// using only the first thread if can't process files in pairs
		if (!(index + 1 < file_names_length)) {
			if (!error1) { pthread_join(thread1, NULL); }
			index += 1;
			continue;
		}

		// creating arguments for the second thread
		struct GrepFileArguments arguments2;
		arguments2.file_name = file_names[index + 1];
		arguments2.options = options;
		// starting second thread for files with odd index
		error2 = pthread_create(&thread2, NULL, &pthread_grep_file, (void*)&arguments2);
		if (error2) { printf("Error: Failed to create thread2."); };

		// waiting for both threads to finish
		if (!error1) { pthread_join(thread1, NULL); }
		if (!error2) { pthread_join(thread2, NULL); }
		index += 2;
	}

	// destroying threads mutex
	pthread_mutex_destroy(&mutex);

	return EXIT_SUCCESS;
}
