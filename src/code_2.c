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

static struct UnkLinkedListNode *u_listHead;
static struct UnkLinkedListNode *u_allNodes;
static struct UnkLinkedListNode *u_currNode;
static struct UnkLinkedListNode *unusedVar;
static struct UnkLinkedListNode *u_linkedLists[20];

void dummy_callback(struct Ape *a, int b) {}

void u_init_some_linked_list(struct UnkLinkedListNode *nodes, int count)
{
	u8 dummy[8];
	struct UnkLinkedListNode *r8 = NULL;
	int i;

	u_currNode = nodes;
	unusedVar = nodes;
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
		u_linkedLists[i] = &nodes[i];
		nodes[i].callback = dummy_callback;
		nodes[i].next = NULL;
		nodes[i].prev = NULL;
	}
	u_allNodes = nodes;
	u_listHead = &nodes[i];
	nodes[i].prev = NULL;
}

void u_process_some_linked_lists(u32 arg0)
{
	int i;
	struct UnkLinkedListNode *next;
	struct UnkLinkedListNode *node;

	for (i = 0; i < 16; i++)
	{
		if (!(arg0 & (1 << i)))
		{
			node = u_linkedLists[i];
			while (node != NULL)
			{
				u_currNode = node;
				next = node->next;
				node->callback(node->ape, 0);
				node = next;
			}
		}
	}
}

int u_insert_into_linked_list(void (*func)(struct Ape *, int), struct Ape *ape, int listId)
{
    struct UnkLinkedListNode *oldHead = u_listHead;

    u_listHead = oldHead->next;
    u_listHead->prev = NULL;
    oldHead->next = u_linkedLists[listId]->next;
    if (oldHead->next != NULL)
        oldHead->next->prev = oldHead;
    u_linkedLists[listId]->next = oldHead;
    oldHead->prev = u_linkedLists[listId];
    oldHead->ape = ape;
    oldHead->callback = func;
    return oldHead->unk18;
}

void u_move_curr_node_to_beginning(void)
{
    struct UnkLinkedListNode *prev = u_currNode->prev;
    struct UnkLinkedListNode *next = u_currNode->next;

    // remove from list
    if (prev != NULL)
        prev->next = next;
    if (next != NULL)
        next->prev = prev;

    // insert at beginning
    u_currNode->prev = NULL;
    u_currNode->next = u_listHead;
    if (u_listHead != NULL)
        u_listHead->prev = u_currNode;
    u_listHead = u_currNode;
}

void u_move_node_to_beginning(int arg0)
{
    struct UnkLinkedListNode *backupCurr = u_currNode;
    struct UnkLinkedListNode *node = &u_allNodes[arg0];
    struct UnkLinkedListNode *prev = node->prev;
    struct UnkLinkedListNode *next = node->next;

    u_currNode = node;

    // remove from list
    if (prev != NULL)
        prev->next = next;
    if (next != NULL)
        next->prev = prev;

    // insert at beginning
    node->prev = NULL;
    node->next = u_listHead;
    if (u_listHead != NULL)
        u_listHead->prev = node;
    u_listHead = node;

    node->callback(node->ape, 3);

    u_currNode = backupCurr;
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
