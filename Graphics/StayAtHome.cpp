
/*
#include "StayAtHome.h"
#include "NPC.h"

void StayAtHome::OnEnter(NPC* pn)
{
	pn->setIsResting(true);
}

void StayAtHome::Transition(NPC* pn)
{
	OnExit(pn);
	pn->setCurrentState(pn->getInterruptedState());
	pn->getCurrentState()->OnEnter(pn);
}

void StayAtHome::OnExit(NPC* pn)
{
	pn->setIsResting(false);
}
*/