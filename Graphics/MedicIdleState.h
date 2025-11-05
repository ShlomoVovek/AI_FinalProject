#pragma once
#include "MedicState.h"

class Medic;

// State: Medic waiting for orders
class MedicIdleState : public MedicState
{
public:
    void OnEnter(Medic* agent) override;
    void Execute(Medic* agent) override;
    void OnExit(Medic* agent) override;
    const char* GetStateName() const override { return "IDLE"; }
};