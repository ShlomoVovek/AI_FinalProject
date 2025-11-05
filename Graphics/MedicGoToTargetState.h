#pragma once
#include "MedicState.h"

class Medic;

// State: Medic moving to injured soldier (target)
class MedicGoToTargetState : public MedicState
{
private:
    bool pathCalculated;

public:
    MedicGoToTargetState() : pathCalculated(false) {}

    void OnEnter(Medic* agent) override;
    void Execute(Medic* agent) override;
    void OnExit(Medic* agent) override;
    const char* GetStateName() const override { return "GO_TO_TARGET"; }
};