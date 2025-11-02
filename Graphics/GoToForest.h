#pragma once
#include "State.h"
#include "NPC.h"

const int FOREST_X = 85;
const int FOREST_Y = 85;


class NPC;

class GoToForest :    public State
{
public:
	void OnEnter(NPC* pn);
	void Transition(NPC* pn);
	void OnExit(NPC* pn);

};

