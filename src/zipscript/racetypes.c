#include "racetypes.h"

void
set_rtype_msg(struct _racetype *dest, const struct _racetype *target)
{
	dest->race = target->race;
	dest->newleader = target->newleader;
	dest->update = target->update;
	dest->race_halfway = target->race_halfway;
	dest->norace_halfway = target->norace_halfway;		
}

