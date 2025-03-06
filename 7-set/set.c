#include "set.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static size_t hash(const char* string, const size_t hash_table_size) {
	return strlen(string) % hash_table_size;
}

Set* set_new(const size_t hash_table_size) {
	Set* set = malloc(sizeof(set));
	if (set == NULL) { return NULL; }
	set->hash_table = malloc(hash_table_size * sizeof(List*));
	if (set->hash_table == NULL) { free(set); return NULL; }
	set->hash_table_size = hash_table_size;

	for (size_t index1 = 0; index1 < hash_table_size; index1++) {
		set->hash_table[index1] = list_new();
		if (set->hash_table[index1] == NULL) {
			for (size_t index2 = 0; index2 < index1; index2++) {
				free(set->hash_table[index2]);
			}
			free(set->hash_table);
			free(set);
			return NULL;
		}
	}

	return set;
}

void set_add(Set* set, const char* string) {
	size_t string_hash = hash(string, set->hash_table_size);
	List* bucket = set->hash_table[string_hash];
	if (list_find(bucket, (void*)string)) { return; }
	list_push(bucket, (void*)string);
}

bool set_find(const Set* set, const char* string) {
	size_t string_hash = hash(string, set->hash_table_size);
	List* bucket = set->hash_table[string_hash];
	return list_find(bucket, (void*)string);
}

void set_print(const Set* set) {
	for (size_t index = 0; index < set->hash_table_size; index++) {
		printf("%zu:", index);

		ListNode* list_node = (set->hash_table[index])->head;
		while (list_node != NULL) {
			printf(" %s,", (char*)list_node->data);
			list_node = list_node->next;
		}
		printf("\n");
	}
}

void set_free(Set* set) {
	if (set == NULL) { return; }
	for (size_t index = 0; index < set->hash_table_size; index++) {
		 list_free(set->hash_table[index]);
	}
	free(set->hash_table);
	free(set);
}
