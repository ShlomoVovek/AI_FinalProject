#pragma once

class NPC;

class State
{
public:
	virtual void OnEnter(NPC* pn) = 0;  // pure virtual
	virtual void Transition(NPC* pn) = 0;  // pure virtual
	virtual void OnExit(NPC* pn) = 0;  // pure virtual
};

