#pragma once
#include "SupplyAgentState.h"

class SupplyAgent;

// State: SupplyAgent has ammo and is waiting in a safe position for a mission
class SupplyWaitState : public SupplyAgentState
{
private:
    bool positionFound;

public:
    SupplyWaitState() : positionFound(false) {}

    void OnEnter(SupplyAgent* agent) override;
    void Execute(SupplyAgent* agent) override;
    void OnExit(SupplyAgent* agent) override;
    const char* GetStateName() const override { return "WAIT_SAFE"; }
};