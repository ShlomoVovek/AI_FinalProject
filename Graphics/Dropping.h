#pragma once
#include "State.h"
class Dropping :
    public State
{
	void OnEnter(NPC* pn);
	void Transition(NPC* pn);
	void OnExit(NPC* pn);

};

