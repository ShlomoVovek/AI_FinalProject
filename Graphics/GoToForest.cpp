/*
#include "GoToForest.h"
#include "Chopping.h"
#include "GoHome.h"

void GoToForest::OnEnter(NPC* pn) // initialization
{
	pn->setTarget(FOREST_X, FOREST_Y);
	pn->setDirection();
	pn->setIsMoving(true);

}

void GoToForest::Transition(NPC* pn)
{
	OnExit(pn); // from current state
	if (pn->getIsTired()) {
		pn->setCurrentState(new GoHome());
		pn->setInterruptedState(this);
	}

	else	pn->setCurrentState(new Chopping()); // create new State
	pn->getCurrentState()->OnEnter(pn); // initiate new State
}

void GoToForest::OnExit(NPC* pn) // turn light off
{
	pn->setIsMoving(false);
}
*/