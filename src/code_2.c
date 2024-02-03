#include <stddef.h>
#include <dolphin.h>

#include "global.h"
#include "nl2ngc.h"

float u_interpolate_other_keyframes(float arg0, struct OtherKeyframe *arg1)
{
    float temp_f10;
    float temp_f11;
    float temp_f12;
    float temp_f13;
    float ret;
    struct OtherKeyframe *phi_r6;

    if (arg0 < arg1->unk0)
        return arg1->unk4;

    phi_r6 = arg1 + 1;
    while (arg0 > phi_r6->unk0)
    {
		if (phi_r6->unk0 == 0xFFFF)
		{
			phi_r6--;
			break;
		}
		phi_r6++;
	}

    temp_f10 = phi_r6->unk0 - phi_r6[-1].unk0;
    temp_f11 = (arg0 - phi_r6[-1].unk0) / temp_f10;
    temp_f12 = temp_f11 * temp_f11;
    temp_f13 = temp_f12 * temp_f11;
    ret = phi_r6[-1].unk4 * (2.0 * temp_f13 - 3.0 * temp_f12 + 1.0)
        + phi_r6->unk4 * (-2.0 * temp_f13 + 3.0 * temp_f12);
    ret += temp_f10 * (phi_r6[-1].unkC * (temp_f13 - 2.0 * temp_f12 + temp_f11) + phi_r6->unk8 * (temp_f13 - temp_f12));
    return ret;
}

/* thread.c */

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

void u_set_model_mesh_flags(struct NlModel *model, u32 arg1, u32 arg2)
{
    struct NlMesh *mesh = (struct NlMesh *)model->meshStart;

    while ((u32)mesh->flags != 0)
	{
		mesh->flags &= arg1;
		mesh->flags |= arg2;
		mesh = (struct NlMesh *)(mesh->dispListStart + mesh->dispListSize);
	}
}

void u_set_model_mesh_unk_flags(struct NlModel *model, u32 arg1, u32 arg2)
{
	struct NlMesh *mesh = (struct NlMesh *)model->meshStart;

	while ((u32)mesh->flags != 0)
	{
		mesh->unk4 &= arg1;
		mesh->unk4 |= arg2;
		mesh = (struct NlMesh *)(mesh->dispListStart + mesh->dispListSize);
	}
}

void u_set_model_mesh_texflags(struct NlModel *model, u32 arg1, u32 arg2)
{
	struct NlMesh *mesh = (struct NlMesh *)model->meshStart;

	while ((u32)mesh->flags != 0)
	{
		mesh->texFlags &= arg1;
		mesh->texFlags |= arg2;
		mesh = (struct NlMesh *)(mesh->dispListStart + mesh->dispListSize);
	}
}
