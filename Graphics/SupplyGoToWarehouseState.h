#pragma once
#include "SupplyAgentState.h"

class SupplyAgent;

// State: SupplyAgent moving to ammo warehouse
class SupplyGoToWarehouseState : public SupplyAgentState
{
private:
    bool pathCalculated;

public:
    SupplyGoToWarehouseState() : pathCalculated(false) {}

    void OnEnter(SupplyAgent* agent) override;
    void Execute(SupplyAgent* agent) override;
    void OnExit(SupplyAgent* agent) override;
    const char* GetStateName() const override { return "GO_TO_WAREHOUSE"; }
};