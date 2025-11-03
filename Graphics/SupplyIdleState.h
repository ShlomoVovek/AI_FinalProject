#pragma once
#include "SupplyAgentState.h"

class SupplyAgent;

// State: SupplyAgent waiting for resupply orders
class SupplyIdleState : public SupplyAgentState
{
public:
    void OnEnter(SupplyAgent* agent) override;
    void Execute(SupplyAgent* agent) override;
    void OnExit(SupplyAgent* agent) override;
    const char* GetStateName() const override { return "IDLE"; }
};