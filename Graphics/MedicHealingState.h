#pragma once
#include "MedicState.h"

class Medic;

// State: Medic healing the target
class MedicHealingState : public MedicState
{
private:
    int healingTimer; // Frames to spend healing

public:
    MedicHealingState() : healingTimer(60) {} // ~2 seconds at 30 FPS
    const double HEAL_AMOUNT_PER_TICK = 25.0;

    void OnEnter(Medic* agent) override;
    void Execute(Medic* agent) override;
    void OnExit(Medic* agent) override;
    const char* GetStateName() const override { return "HEALING"; }
};