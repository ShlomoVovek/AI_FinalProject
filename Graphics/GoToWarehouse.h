#pragma once
#include "State.h"
#include "NPC.h"

const int WAREHOUSE_X = 25;
const int WAREHOUSE_Y = 25;


class NPC;
class GoToWarehouse :
    public State
{
	void OnEnter(NPC* pn);
	void Transition(NPC* pn);
	void OnExit(NPC* pn);

};

