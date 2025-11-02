/*
#include "Chopping.h"
#include "NPC.h"
#include "GoToWarehouse.h"
#include "GoHome.h"
#include "GoToForest.h"

void Chopping::OnEnter(NPC* pn)
{
	pn->setIsChopping(true);
}

void Chopping::Transition(NPC* pn)
{
	OnExit(pn);
	if (pn->getIsTired())
	{
		pn->setCurrentState(new GoHome());
		pn->setInterruptedState(new GoToForest());
	}
	else pn->setCurrentState(new GoToWarehouse());
	pn->getCurrentState()->OnEnter(pn);
}

void Chopping::OnExit(NPC* pn)
{
	pn->setIsChopping(false);

}
*/