#pragma once
#include "State.h"

const int HOME_X = 15;
const int HOME_Y = 75;


class GoHome :
    public State
{
	void OnEnter(NPC* pn);
	void Transition(NPC* pn);
	void OnExit(NPC* pn);

};

