#include <dolphin/os.h>
#include <ppcintrinsic.h>
#include <stddef.h>

#include "__os.h"

static volatile u32 RunQueueBits;
static volatile BOOL RunQueueHint;
static volatile s32 Reschedule;

static OSThreadQueue RunQueue[32];
static OSThread IdleThread;
static OSThread DefaultThread;
static OSContext IdleContext;

#ifdef __MWERKS__
OSThread *__OSCurrentThread : OS_BASE_CACHED + 0x00E4;
volatile OSContext *__OSFPUContext : OS_BASE_CACHED + 0x00D8;
OSThreadQueue __OSActiveThreadQueue : OS_BASE_CACHED + 0x00DC;
#else
#define __OSCurrentThread (*(OSThread **)(OS_BASE_CACHED + 0x00E4))
#define __OSFPUContext (*(volatile OSContext **)(OS_BASE_CACHED + 0x00D8))
#define __OSActiveThreadQueue (*(OSThreadQueue *)(OS_BASE_CACHED + 0x00DC))
#endif

extern u8 _stack_addr[];
extern u8 _stack_end[];

#define AddTail(queue, thread, link)                                                                                   \
    do                                                                                                                 \
    {                                                                                                                  \
        OSThread *prev;                                                                                                \
                                                                                                                       \
        prev = (queue)->tail;                                                                                          \
        if (prev == NULL)                                                                                              \
            (queue)->head = (thread);                                                                                  \
        else                                                                                                           \
            prev->link.next = (thread);                                                                                \
        (thread)->link.prev = prev;                                                                                    \
        (thread)->link.next = NULL;                                                                                    \
        (queue)->tail = (thread);                                                                                      \
    } while (0)

#define AddPrio(queue, thread, link)                                                                                   \
    do                                                                                                                 \
    {                                                                                                                  \
        OSThread *prev, *next;                                                                                         \
                                                                                                                       \
        for (next = (queue)->head; next && next->priority <= thread->priority; next = next->link.next)                 \
            ;                                                                                                          \
        if (next == NULL)                                                                                              \
            AddTail(queue, thread, link);                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            (thread)->link.next = next;                                                                                \
            prev = next->link.prev;                                                                                    \
            next->link.prev = (thread);                                                                                \
            (thread)->link.prev = prev;                                                                                \
            if (prev == NULL)                                                                                          \
                (queue)->head = (thread);                                                                              \
            else                                                                                                       \
                prev->link.next = (thread);                                                                            \
        }                                                                                                              \
    } while (0)

#define RemoveItem(queue, thread, link)                                                                                \
    do                                                                                                                 \
    {                                                                                                                  \
        OSThread *next, *prev;                                                                                         \
        next = (thread)->link.next;                                                                                    \
        prev = (thread)->link.prev;                                                                                    \
        if (next == NULL)                                                                                              \
            (queue)->tail = prev;                                                                                      \
        else                                                                                                           \
            next->link.prev = prev;                                                                                    \
        if (prev == NULL)                                                                                              \
            (queue)->head = next;                                                                                      \
        else                                                                                                           \
            prev->link.next = next;                                                                                    \
    } while (0)

#define RemoveHead(queue, thread, link)                                                                                \
    do                                                                                                                 \
    {                                                                                                                  \
        OSThread *__next;                                                                                              \
        (thread) = (queue)->head;                                                                                      \
        __next = (thread)->link.next;                                                                                  \
        if (__next == NULL)                                                                                            \
            (queue)->tail = NULL;                                                                                      \
        else                                                                                                           \
            __next->link.prev = NULL;                                                                                  \
        (queue)->head = __next;                                                                                        \
    } while (0)

static inline void OSInitMutexQueue(OSMutexQueue *queue)
{
    queue->head = queue->tail = NULL;
}

void __OSThreadInit()
{
    OSThread *thread = &DefaultThread;
    int prio;

    thread->state = OS_THREAD_STATE_RUNNING;
    thread->attr = OS_THREAD_ATTR_DETACH;
    thread->priority = thread->base = 16;
    thread->suspend = 0;
    thread->val = (void *)-1;
    thread->mutex = NULL;
    OSInitThreadQueue(&thread->queueJoin);
    OSInitMutexQueue(&thread->queueMutex);

    __OSFPUContext = &thread->context;

    OSClearContext(&thread->context);
    OSSetCurrentContext(&thread->context);
    thread->stackBase = (void *)_stack_addr;
    thread->stackEnd = (void *)_stack_end;
    *(thread->stackEnd) = OS_THREAD_STACK_MAGIC;

    RunQueueBits = 0;
    __OSCurrentThread = thread;
    RunQueueHint = FALSE;
    for (prio = OS_PRIORITY_MIN; prio <= OS_PRIORITY_MAX; ++prio)
        OSInitThreadQueue(&RunQueue[prio]);

    OSInitThreadQueue(&__OSActiveThreadQueue);
    AddTail(&__OSActiveThreadQueue, thread, linkActive);
    OSClearContext(&IdleContext);
    Reschedule = 0;
}

void OSInitThreadQueue(OSThreadQueue *queue)
{
    queue->head = queue->tail = NULL;
}

OSThread *OSGetCurrentThread()
{
    return __OSCurrentThread;
}

s32 OSDisableScheduler()
{
    BOOL enabled;
    s32 count;

    enabled = OSDisableInterrupts();
    count = Reschedule++;
    OSRestoreInterrupts(enabled);
    return count;
}

s32 OSEnableScheduler()
{
    BOOL enabled;
    s32 count;

    enabled = OSDisableInterrupts();
    count = Reschedule--;
    OSRestoreInterrupts(enabled);
    return count;
}

static void SetRun(OSThread *thread)
{
    thread->queue = &RunQueue[thread->priority];
    AddTail(thread->queue, thread, link);
    RunQueueBits |= 1u << (OS_PRIORITY_MAX - thread->priority);
    RunQueueHint = TRUE;
}

#pragma dont_inline on
static void UnsetRun(OSThread *thread)
{
    OSThreadQueue *queue;
    queue = thread->queue;
    RemoveItem(queue, thread, link);
    if (queue->head == 0)
        RunQueueBits &= ~(1u << (OS_PRIORITY_MAX - thread->priority));
    thread->queue = NULL;
}
#pragma dont_inline reset

OSPriority __OSGetEffectivePriority(OSThread *thread)
{
    OSPriority priority;
    OSMutex *mutex;
    OSThread *blocked;

    priority = thread->base;
    for (mutex = thread->queueMutex.head; mutex; mutex = mutex->link.next)
    {
        blocked = mutex->queue.head;
        if (blocked && blocked->priority < priority)
            priority = blocked->priority;
    }
    return priority;
}

static OSThread *SetEffectivePriority(OSThread *thread, OSPriority priority)
{
    switch (thread->state)
    {
    case OS_THREAD_STATE_READY:
        UnsetRun(thread);
        thread->priority = priority;
        SetRun(thread);
        break;
    case OS_THREAD_STATE_WAITING:
        RemoveItem(thread->queue, thread, link);
        thread->priority = priority;
        AddPrio(thread->queue, thread, link);
        if (thread->mutex)
            return thread->mutex->thread;
        break;
    case OS_THREAD_STATE_RUNNING:
        RunQueueHint = TRUE;
        thread->priority = priority;
        break;
    }
    return NULL;
}

static void UpdatePriority(OSThread *thread)
{
    OSPriority priority;

    do
    {
        if (0 < thread->suspend)
            break;
        priority = __OSGetEffectivePriority(thread);
        if (thread->priority == priority)
            break;
        thread = SetEffectivePriority(thread, priority);
    } while (thread);
}

static void __OSSwitchThread(OSThread *nextThread)
{
    __OSCurrentThread = nextThread;
    OSSetCurrentContext(&nextThread->context);
    OSLoadContext(&nextThread->context);
}

static OSThread *SelectThread(BOOL yield)
{
    OSContext *currentContext;
    OSThread *currentThread;
    OSThread *nextThread;
    OSPriority priority;
    OSThreadQueue *queue;

    if (0 < Reschedule)
        return 0;

    currentContext = OSGetCurrentContext();
    currentThread = OSGetCurrentThread();
    if (currentContext != &currentThread->context)
        return 0;

    if (currentThread)
    {
        if (currentThread->state == OS_THREAD_STATE_RUNNING)
        {
            if (!yield)
            {
                priority = __cntlzw(RunQueueBits);
                if (currentThread->priority <= priority)
                    return 0;
            }
            currentThread->state = OS_THREAD_STATE_READY;
            SetRun(currentThread);
        }

        if (!(currentThread->context.state & OS_CONTEXT_STATE_EXC) && OSSaveContext(&currentThread->context))
            return 0;
    }

    __OSCurrentThread = NULL;
    if (RunQueueBits == 0)
    {
        OSSetCurrentContext(&IdleContext);
        do
        {
            OSEnableInterrupts();
            while (RunQueueBits == 0)
                ;
            OSDisableInterrupts();
        } while (RunQueueBits == 0);

        OSClearContext(&IdleContext);
    }

    RunQueueHint = FALSE;

    priority = __cntlzw(RunQueueBits);
    queue = &RunQueue[priority];
    RemoveHead(queue, nextThread, link);
    if (queue->head == 0)
        RunQueueBits &= ~(1u << (OS_PRIORITY_MAX - priority));
    nextThread->queue = NULL;
    nextThread->state = OS_THREAD_STATE_RUNNING;
    __OSSwitchThread(nextThread);
    return nextThread;
}

void __OSReschedule()
{
    if (!RunQueueHint)
        return;
    SelectThread(FALSE);
}

void OSCancelThread(OSThread *thread)
{
    BOOL enabled;

    enabled = OSDisableInterrupts();

    switch (thread->state)
    {
    case OS_THREAD_STATE_READY:
        if (!(0 < thread->suspend))
            UnsetRun(thread);
        break;
    case OS_THREAD_STATE_RUNNING: RunQueueHint = TRUE; break;
    case OS_THREAD_STATE_WAITING:
        RemoveItem(thread->queue, thread, link);
        thread->queue = NULL;
        if (!(0 < thread->suspend) && thread->mutex)
            UpdatePriority(thread->mutex->thread);
        break;
    default: OSRestoreInterrupts(enabled); return;
    }

    OSClearContext(&thread->context);
    if (thread->attr & OS_THREAD_ATTR_DETACH)
    {
        RemoveItem(&__OSActiveThreadQueue, thread, linkActive);
        thread->state = 0;
    }
    else
        thread->state = OS_THREAD_STATE_MORIBUND;

    __OSUnlockAllMutex(thread);

    OSWakeupThread(&thread->queueJoin);

    __OSReschedule();
    OSRestoreInterrupts(enabled);

    return;
}

s32 OSResumeThread(OSThread *thread)
{
    BOOL enabled;
    s32 suspendCount;

    enabled = OSDisableInterrupts();
    suspendCount = thread->suspend--;
    if (thread->suspend < 0)
        thread->suspend = 0;
    else if (thread->suspend == 0)
    {
        switch (thread->state)
        {
        case OS_THREAD_STATE_READY:
            thread->priority = __OSGetEffectivePriority(thread);
            SetRun(thread);
            break;
        case OS_THREAD_STATE_WAITING:
            RemoveItem(thread->queue, thread, link);
            thread->priority = __OSGetEffectivePriority(thread);
            AddPrio(thread->queue, thread, link);
            if (thread->mutex)
                UpdatePriority(thread->mutex->thread);
            break;
        }
        __OSReschedule();
    }
    OSRestoreInterrupts(enabled);
    return suspendCount;
}

s32 OSSuspendThread(OSThread *thread)
{
    BOOL enabled;
    s32 suspendCount;

    enabled = OSDisableInterrupts();
    suspendCount = thread->suspend++;
    if (suspendCount == 0)
    {
        switch (thread->state)
        {
        case OS_THREAD_STATE_RUNNING:
            RunQueueHint = TRUE;
            thread->state = OS_THREAD_STATE_READY;
            break;
        case OS_THREAD_STATE_READY: UnsetRun(thread); break;
        case OS_THREAD_STATE_WAITING:
            RemoveItem(thread->queue, thread, link);
            thread->priority = 32;
            AddTail(thread->queue, thread, link);
            if (thread->mutex)
                UpdatePriority(thread->mutex->thread);
            break;
        }

        __OSReschedule();
    }
    OSRestoreInterrupts(enabled);
    return suspendCount;
}

void OSSleepThread(OSThreadQueue *queue)
{
    BOOL enabled;
    OSThread *currentThread;

    enabled = OSDisableInterrupts();
    currentThread = OSGetCurrentThread();

    currentThread->state = OS_THREAD_STATE_WAITING;
    currentThread->queue = queue;
    AddPrio(queue, currentThread, link);
    RunQueueHint = TRUE;
    __OSReschedule();
    OSRestoreInterrupts(enabled);
}

void OSWakeupThread(OSThreadQueue *queue)
{
    BOOL enabled;
    OSThread *thread;

    enabled = OSDisableInterrupts();
    while (queue->head)
    {
        RemoveHead(queue, thread, link);
        thread->state = OS_THREAD_STATE_READY;
        if (!(0 < thread->suspend))
            SetRun(thread);
    }
    __OSReschedule();
    OSRestoreInterrupts(enabled);
}
