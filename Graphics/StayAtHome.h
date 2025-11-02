#pragma once
#include "State.h"
class StayAtHome :
    public State
{
	void OnEnter(NPC* pn);
	void Transition(NPC* pn);
	void OnExit(NPC* pn);
};

