#ifndef _SRC_THREAD_H_
#define _SRC_THREAD_H_

struct Thread
{
	void (*callback)(struct Ape *, int);
	struct Ape *ape;
	struct Thread *next;
	struct Thread *prev;
	u32 unk10;
	u32 unk14;
	u32 unk18;
	u32 unk1C;
};

void thread_init(struct Thread *nodes, int count);
void thread_loop(u32 arg0);
int thread_unknown(void (*func)(struct Ape *, int), struct Ape *ape, int listId);
void thread_exit(void);
void thread_kill(int arg0);

#endif
