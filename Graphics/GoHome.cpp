/*
#include "GoHome.h"
#include "NPC.h"
#include "StayAtHome.h"

void GoHome::OnEnter(NPC* pn)
{
	pn->setTarget(HOME_X, HOME_Y);
	pn->setDirection();
	pn->setIsMoving(true);


}

void GoHome::Transition(NPC* pn)
{
	OnExit(pn); // from current state
	pn->setCurrentState(new StayAtHome()); // create new State
	pn->getCurrentState()->OnEnter(pn); // initiate new State

}

void GoHome::OnExit(NPC* pn)
{
	pn->setIsMoving(false);

}
*/
