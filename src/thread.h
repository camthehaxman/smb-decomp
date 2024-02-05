#ifndef _SRC_THREAD_H_
#define _SRC_THREAD_H_

enum ThreadStatus
{
    THREAD_STATUS_RUNNING = 0,
    THREAD_STATUS_KILLED = 3,
};

enum ThreadGroup
{
    THREAD_GROUP_0,
    THREAD_GROUP_1,
    THREAD_GROUP_2,
    THREAD_GROUP_3,
    THREAD_GROUP_4,
    THREAD_GROUP_5,
    THREAD_GROUP_6,
    THREAD_GROUP_7,
    THREAD_GROUP_8,
    THREAD_GROUP_9,
    THREAD_GROUP_10,
    THREAD_GROUP_11,
    THREAD_GROUP_12,
    THREAD_GROUP_13,
    THREAD_GROUP_14,
    THREAD_GROUP_15,
    THREAD_GROUP_COUNT,
};

#define ALL_THREAD_MASK 0xFFFFFF

typedef void (*ThreadCallback)(struct Ape *, int);

struct Thread
{
    ThreadCallback callback;
    struct Ape *ape;
    struct Thread *next;
    struct Thread *prev;
    u32 unused1;
    u32 unused2;
    u32 threadId;  // unique ID of the thread (index into s_threadWork array)
    u32 unused3;
};

void thread_init(struct Thread *work, int count);
void thread_loop(u32 excludeMask);
int thread_create(ThreadCallback func, struct Ape *ape, enum ThreadGroup group);
void thread_exit(void);
void thread_kill(int threadId);

#endif
