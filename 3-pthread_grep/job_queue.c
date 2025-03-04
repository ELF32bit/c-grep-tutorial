#include "job_queue.h"

#include <stdlib.h>

JobQueue* job_queue_new() {
	JobQueue* queue = malloc(sizeof(JobQueue));
	if (queue == NULL) { return NULL; }
	if (pthread_mutex_init(&queue->mutex, NULL)) {
		free(queue);
		return NULL;
	}
	queue->head = NULL;
	queue->tail = NULL;
	return queue;
}

void job_queue_push(JobQueue* queue, void* task) {
	if (queue->tail == NULL) {
		queue->tail = malloc(sizeof(Job));
		if (queue->tail == NULL) { return; }
		queue->tail->task = task;
		queue->tail->next = NULL;
		queue->head = queue->tail;
	} else {
		queue->tail->next = malloc(sizeof(Job));
		if (queue->tail->next == NULL) { return; }
		queue->tail = queue->tail->next;
		queue->tail->task = task;
		queue->tail->next = NULL;
	}
}

void* job_queue_pop(JobQueue* queue) {
	pthread_mutex_lock(&queue->mutex);
	void* task = NULL;
	if (queue->head != NULL) {
		Job* job = queue->head;
		task = queue->head->task;
		if (queue->head == queue->tail) {
			queue->head = NULL;
			queue->tail = NULL;
		} else {
			queue->head = queue->head->next;
		}
		free(job);
	}
	pthread_mutex_unlock(&queue->mutex);
	return task;
}

void job_queue_free(JobQueue* queue) {
	if (queue == NULL) { return; }
	while (queue->head != NULL) {
		Job* job = queue->head;
		void* task = queue->head->task;
		if (task != NULL) { free(task); };
		queue->head = queue->head->next;
		free(job);
	}
	pthread_mutex_destroy(&queue->mutex);
	free(queue);
}
