#include "grep.h"

#include <stdlib.h> // EXIT_SUCCESS, EXIT_FAILURE
#include <stdio.h> // asprintf(), fopen(), getline()
#include <string.h> // strlen(), strdup()
#include <ctype.h> // toupper(), isalpha()

#include <pthread.h>

// this structure is not declared in the header file, it's local to this file
struct GrepFileArguments {
	char* file_name;
	const GrepOptions* options;
};

// this function is not declared in the header file, it's local to this file
void *pthread_grep_file(void* arguments) {
	// casting pointer to arguments structure from void* to its proper type
	struct GrepFileArguments* args = (struct GrepFileArguments*) arguments;
	GrepResult* grep_result = grep_file(args->file_name, args->options);
	printf("(%s): %zu\n", args->file_name, grep_result->match_count);
	grep_result_free(grep_result);
	return NULL;
}

int grep_files(char** file_names, int file_names_length, const GrepOptions* options) {
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
