#ifndef LIST_H
#define LIST_H

typedef int bool;

typedef struct ListNode {
	void* data;
	struct ListNode* next;
} ListNode;

typedef struct List {
	ListNode* head;
} List;

List* list_new();
void list_push(List* list, void* data);
bool list_find(const List* list, void* data);
void list_free(List* list);

#endif
