#pragma once
#include "State.h"

const int MAX_WOODS = 1000;

class NPC;

class Chopping :
    public State
{
	void OnEnter(NPC* pn);
	void Transition(NPC* pn);
	void OnExit(NPC* pn);

};

