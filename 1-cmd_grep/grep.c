#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_RESET "\x1b[0m"

struct Options {
	int ignore_case;
	char* search_string;
	char* file_name;
};

int grep(struct Options options) {
	// 1. Trying to open input file for reading
	FILE *file = fopen(options.file_name, "r");
	if (file == NULL) {
		printf("Error: No such file.\n");
		return 1;
	}

	//2. size_t stores max array size on x64 or x86 system
	size_t index = 0;
	char* search_string = options.search_string;
	size_t search_string_length = strlen(options.search_string);

	// duplicating search string and making it uppercase
	if (options.ignore_case) {
		search_string = strdup(options.search_string);
		for (index = 0; index < search_string_length; index++) {
			search_string[index] = toupper(search_string[index]);
		}
	}

	// storing matching substring inside buffer string
	char* buffer_string = strdup(options.search_string);
	size_t match_index = 0;

	//3.
	int c; // fgetc returns int instead of char
	while ((c = fgetc(file)) != EOF) {
		int cc = options.ignore_case ? toupper(c) : c;

		if (match_index == search_string_length - 1) {
			match_index = 0;
			printf(ANSI_COLOR_RED);
			printf("%s", buffer_string);
			printf(ANSI_COLOR_RESET);
		}
		else if (cc == search_string[match_index]) {
			buffer_string[match_index] = c;
			match_index += 1;
		}
		else {
			for (index = 0; index < match_index; index++) {
				printf("%c", buffer_string[index]);
			}
			match_index = 0;
			printf("%c", c);
		}
	}

	return 0;
}

int main(int argc, char **argv) {
	struct Options options;

	//1.
	// getopt from unistd.h is commonly used to parse input options
	// option (:e) can be followed by an additional argument (optarg)
	char c;
	while ((c = getopt (argc, argv, "hi")) != -1) {
		switch (c) {
			case 'i':
				options.ignore_case = 1;
				break;
			case 'h':
				printf("Search for PATTERN in FILE.\n");
				printf("Usage: grep [OPTIONS] PATTERN FILE\n");
				printf("Example: grep -i 'hello world' main.c\n");
				return 0;
		}
	}
	// (optind) is a global variable from unistd.h
	// it stores index of the first non-option argument
	if (optind + 1 < argc) {
		options.search_string = argv[optind + 0];
		options.file_name = argv[optind + 1];
	} else {
		printf("Error: Bad arguments.\n");
	}

	//2. Printing options.
	printf("Searching in: %s\n", options.file_name);
	printf("Searching for: %s\n", options.search_string);
	printf("Searching ignoring case: %s\n", options.ignore_case ? "TRUE" : "FALSE");

	//3.
	return grep(options);
}
