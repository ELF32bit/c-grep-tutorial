/* All header files need to be protected with preprocessor guards */
#ifndef JOB_QUEUE_H
#define JOB_QUEUE_H

#include <pthread.h> // pthread_mutex_t

/* Always prefix structs with header name */
typedef struct Job {
	void* task;
	struct Job* next;
} Job;

/* Use typedef for structs to improve readability */
typedef struct JobQueue {
	struct Job* head;
	struct Job* tail;
	pthread_mutex_t mutex; /* Mutex prevents race conditions */
} JobQueue;

/* Always prefix functions with header name */
JobQueue* job_queue_new();
void job_queue_push(JobQueue* queue, void* task);
void* job_queue_pop(JobQueue* queue);
void job_queue_free(JobQueue* queue);

#endif
