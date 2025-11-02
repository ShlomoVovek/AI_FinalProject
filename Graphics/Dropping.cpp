/*
#include "Dropping.h"
#include "NPC.h"
#include "GoHome.h"
#include "GoToWarehouse.h"


void Dropping::OnEnter(NPC* pn)
{
	pn->setIsDropping(true);

}

void Dropping::Transition(NPC* pn)
{
	OnExit(pn);
	if (pn->getIsTired()) 
	{
		pn->setCurrentState(new GoHome());
		pn->setInterruptedState(new GoToWarehouse());
	}

	else	pn->setCurrentState(new GoToForest());
	pn->getCurrentState()->OnEnter(pn);

}

void Dropping::OnExit(NPC* pn)
{
}
*/