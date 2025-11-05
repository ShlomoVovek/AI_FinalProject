#pragma once
#include "MedicState.h"

class Medic;

// State: Medic moving to medicine warehouse (base)
class MedicGoToBaseState : public MedicState
{
private:
    bool pathCalculated;

public:
    MedicGoToBaseState() : pathCalculated(false) {}

    void OnEnter(Medic* agent) override;
    void Execute(Medic* agent) override;
    void OnExit(Medic* agent) override;
    const char* GetStateName() const override { return "GO_TO_BASE"; }
};