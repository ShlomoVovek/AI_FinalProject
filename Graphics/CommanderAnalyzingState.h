#pragma once
#include "CommanderState.h"
#include "SupplyAgent.h"
#include "Medic.h"
#include "Warrior.h"
#include "NPC.h"

class Commander;

// State: Commander analyzes visibility maps and processes reports
class CommanderAnalyzingState : public CommanderState
{
public:
    void OnEnter(Commander* commander) override;
    void Execute(Commander* commander) override;
    void OnExit(Commander* commander) override;
    const char* GetStateName() const override { return "ANALYZING"; }

    void HandleSupplyRequests(Commander* commander);
};









