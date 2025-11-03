#pragma once
#include "SupplyAgentState.h"

class SupplyAgent;

// State: SupplyAgent delivering ammo to warrior
class SupplyDeliveringState : public SupplyAgentState
{
private:
    int deliveryTimer; // Frames to spend delivering

public:
    SupplyDeliveringState() : deliveryTimer(30) {} // ~1 second at 30 FPS

    void OnEnter(SupplyAgent* agent) override;
    void Execute(SupplyAgent* agent) override;
    void OnExit(SupplyAgent* agent) override;
    const char* GetStateName() const override { return "DELIVERING"; }
};