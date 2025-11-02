#include "GoToWarehouse.h"
#include "Dropping.h"
#include "GoHome.h"

/*
void GoToWarehouse::OnEnter(NPC* pn)
{
	pn->setTarget(WAREHOUSE_X, WAREHOUSE_Y);
	pn->setDirection();
	pn->setIsMoving(true);


}

void GoToWarehouse::Transition(NPC* pn)
{
	OnExit(pn); // from current state
	if (pn->getIsTired()) {
		pn->setCurrentState(new GoHome());
		pn->setInterruptedState(this);
	}

	else	pn->setCurrentState(new Dropping()); // create new State
	pn->getCurrentState()->OnEnter(pn); // initiate new State

}

void GoToWarehouse::OnExit(NPC* pn)
{
	pn->setIsMoving(false);

}
*/