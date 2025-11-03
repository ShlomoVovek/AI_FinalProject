#pragma once
#include "CommanderState.h"

class Commander;

// State: Commander decides strategy (attack/defend/move)
class CommanderPlanningState : public CommanderState
{
public:
    void OnEnter(Commander* commander) override;
    void Execute(Commander* commander) override;
    void OnExit(Commander* commander) override;
    const char* GetStateName() const override { return "PLANNING"; }
};