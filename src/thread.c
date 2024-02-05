#include <stddef.h>
#include <dolphin.h>

#include "global.h"
#include "thread.h"

static struct Thread *freeThread;
static struct Thread *threadWork;
static struct Thread *currentThread;
static struct Thread *restoreThread;  // set, but not used
static struct Thread *executeLevels[20];

void dummy_thread(struct Ape *a, int b) {}

void thread_init(struct Thread *nodes, int count)
{
	u8 dummy[8];
	struct Thread *r8 = NULL;
	int i;

	currentThread = nodes;
	restoreThread = nodes;
	for (i = 0; i < count; i++)
	{
		nodes[i].callback = NULL;
		nodes[i].ape = NULL;
		nodes[i].unk10 = 0;
		nodes[i].unk14 = 0;
		nodes[i].unk18 = i;
		nodes[i].unk1C = 0;
		nodes[i].prev = r8;
		nodes[i].next = &nodes[i + 1];
		r8 = &nodes[i];
	}
	for (i = 0; i < 16; i++)
	{
		executeLevels[i] = &nodes[i];
		nodes[i].callback = dummy_thread;
		nodes[i].next = NULL;
		nodes[i].prev = NULL;
	}
	threadWork = nodes;
	freeThread = &nodes[i];
	nodes[i].prev = NULL;
}

void thread_loop(u32 arg0)
{
	int i;
	struct Thread *next;
	struct Thread *node;

	for (i = 0; i < 16; i++)
	{
		if (!(arg0 & (1 << i)))
		{
			node = executeLevels[i];
			while (node != NULL)
			{
				currentThread = node;
				next = node->next;
				node->callback(node->ape, 0);
				node = next;
			}
		}
	}
}

int thread_unknown(void (*func)(struct Ape *, int), struct Ape *ape, int listId)
{
    struct Thread *oldHead = freeThread;

    freeThread = oldHead->next;
    freeThread->prev = NULL;
    oldHead->next = executeLevels[listId]->next;
    if (oldHead->next != NULL)
        oldHead->next->prev = oldHead;
    executeLevels[listId]->next = oldHead;
    oldHead->prev = executeLevels[listId];
    oldHead->ape = ape;
    oldHead->callback = func;
    return oldHead->unk18;
}

void thread_exit(void)
{
    struct Thread *prev = currentThread->prev;
    struct Thread *next = currentThread->next;

    // remove from list
    if (prev != NULL)
        prev->next = next;
    if (next != NULL)
        next->prev = prev;

    // insert at beginning
    currentThread->prev = NULL;
    currentThread->next = freeThread;
    if (freeThread != NULL)
        freeThread->prev = currentThread;
    freeThread = currentThread;
}

void thread_kill(int arg0)
{
    struct Thread *backupCurr = currentThread;
    struct Thread *node = &threadWork[arg0];
    struct Thread *prev = node->prev;
    struct Thread *next = node->next;

    currentThread = node;

    // remove from list
    if (prev != NULL)
        prev->next = next;
    if (next != NULL)
        next->prev = prev;

    // insert at beginning
    node->prev = NULL;
    node->next = freeThread;
    if (freeThread != NULL)
        freeThread->prev = node;
    freeThread = node;

    node->callback(node->ape, 3);

    currentThread = backupCurr;
}
