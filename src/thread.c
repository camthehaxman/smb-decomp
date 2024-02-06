/**
 * thread.c - implements a callback system (only used for ape animation processing, I think?).
 *
 * These aren't really threads since there is no concurrency, but more like a callback system.
 * Each thread consists of a function that is run once each frame (when thread_loop is called) until
 * the thread exits (by calling thread_exit) or is killed from outside the thread (by calling thread_kill).
 */
#include <stddef.h>
#include <dolphin.h>

#include "global.h"
#include "thread.h"

static struct Thread *s_freeThread;  // current head of the linked list, which new threads are assigned to
static struct Thread *s_threadWork;  // array of all Thread structures
static struct Thread *s_currentThread;  // currently executing thread
static struct Thread *s_restoreThread;  // (unused) It's called "RestoreThread" in the SMBDX symbol map. It's initialized in thread_init, but never used again.

// array of linked lists, each representing a group of threads
// The first element of each group is a dummy thread, which does nothing.
// New threads are added to the beginning of the group, but after the dummy thread.
static struct Thread *s_threadGroups[THREAD_GROUP_COUNT + 4 /* why the extra space? */];

static void dummy_thread(struct Ape *a, int b) {}

// Initializes the thread system
void thread_init(struct Thread *work, int count)
{
    u8 dummy[8];
    struct Thread *prev = NULL;
    int i;

    s_currentThread = work;
    s_restoreThread = work;

    // Initialize all Thread structs
    for (i = 0; i < count; i++)
    {
	work[i].callback = NULL;
	work[i].ape = NULL;
	work[i].unused1 = 0;
	work[i].unused2 = 0;
	work[i].threadId = i;
	work[i].unused3 = 0;
	// Make this array into a linked list
	work[i].prev = prev;
	work[i].next = &work[i + 1];
	prev = &work[i];
    }

    // Set up thread groups
    // The first thread of each group is a dummy one which does nothing.
    for (i = 0; i < THREAD_GROUP_COUNT; i++)
    {
	s_threadGroups[i] = &work[i];
	work[i].callback = dummy_thread;
	work[i].next = NULL;
	work[i].prev = NULL;
    }

    s_threadWork = work;

    // The rest of the threads are free to be assigned
    s_freeThread = &work[i];
    work[i].prev = NULL;
}

/* Runs all thread groups in order, except the ones specified in excludeMask */
void thread_loop(u32 excludeMask)
{
    int group;
    struct Thread *next;
    struct Thread *curr;

    for (group = 0; group < THREAD_GROUP_COUNT; group++)
    {
	if (excludeMask & (1 << group))
	    continue;
	curr = s_threadGroups[group];
	while (curr != NULL)
	{
	    s_currentThread = curr;
	    next = curr->next;
	    curr->callback(curr->ape, THREAD_STATUS_RUNNING);
	    curr = next;
	}
    }
}

/* Adds the thread to the beginning of the specified thread group. Returns its ID */
int thread_create(ThreadCallback func, struct Ape *ape, enum ThreadGroup group)
{
    struct Thread *new = s_freeThread;

    s_freeThread = s_freeThread->next;
    s_freeThread->prev = NULL;

    // Insert between the first (dummy) thread of the group and the rest of the group
    new->next = s_threadGroups[group]->next;
    if (new->next != NULL)
        new->next->prev = new;
    s_threadGroups[group]->next = new;
    new->prev = s_threadGroups[group];

    new->ape = ape;
    new->callback = func;
    return new->threadId;
}

/* Exits the currently running thread. This function should only be called from a thread callback. */
void thread_exit(void)
{
    struct Thread *prev = s_currentThread->prev;
    struct Thread *next = s_currentThread->next;

    // remove from list
    if (prev != NULL)
        prev->next = next;
    if (next != NULL)
        next->prev = prev;

    // insert at beginning
    s_currentThread->prev = NULL;
    s_currentThread->next = s_freeThread;
    if (s_freeThread != NULL)
        s_freeThread->prev = s_currentThread;
    s_freeThread = s_currentThread;
}

/* Stops the specified thread from running. */
void thread_kill(int threadId)
{
    struct Thread *backupCurr = s_currentThread;
    struct Thread *thread = &s_threadWork[threadId];
    struct Thread *prev = thread->prev;
    struct Thread *next = thread->next;

    s_currentThread = thread;

    // Remove thread from list
    if (prev != NULL)
        prev->next = next;
    if (next != NULL)
        next->prev = prev;

    // Free the thread by inserting it right before s_freeThread
    thread->prev = NULL;
    thread->next = s_freeThread;
    if (s_freeThread != NULL)
        s_freeThread->prev = thread;
    s_freeThread = thread;

    // Notify the thread of its termination
    thread->callback(thread->ape, THREAD_STATUS_KILLED);

    s_currentThread = backupCurr;
}
