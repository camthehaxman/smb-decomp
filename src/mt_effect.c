#include <dolphin.h>

#include "global.h"
#include "nl2ngc.h"
#include "mt_effect.h"

void MTEfcChangeAttributePC(struct NlModel *model, u32 arg1, u32 arg2)
{
    struct NlMesh *mesh = (struct NlMesh *)model->meshStart;

    while ((u32)mesh->flags != 0)
	{
		mesh->flags &= arg1;
		mesh->flags |= arg2;
		mesh = (struct NlMesh *)(mesh->dispListStart + mesh->dispListSize);
	}
}

void MTEfcChangeAttributeITI(struct NlModel *model, u32 arg1, u32 arg2)
{
	struct NlMesh *mesh = (struct NlMesh *)model->meshStart;

	while ((u32)mesh->flags != 0)
	{
		mesh->unk4 &= arg1;
		mesh->unk4 |= arg2;
		mesh = (struct NlMesh *)(mesh->dispListStart + mesh->dispListSize);
	}
}

void MTEfcChangeAttributeTI(struct NlModel *model, u32 arg1, u32 arg2)
{
	struct NlMesh *mesh = (struct NlMesh *)model->meshStart;

	while ((u32)mesh->flags != 0)
	{
		mesh->texFlags &= arg1;
		mesh->texFlags |= arg2;
		mesh = (struct NlMesh *)(mesh->dispListStart + mesh->dispListSize);
	}
}
