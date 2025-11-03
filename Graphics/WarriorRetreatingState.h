#pragma once
#include "WarriorState.h"

class WarriorRetreatingState : public WarriorState
{
private:
    bool safePositionFound;

public:
    WarriorRetreatingState() : safePositionFound(false) {}

    void OnEnter(Warrior* warrior) override;
    void Execute(Warrior* warrior) override;
    void OnExit(Warrior* warrior) override;
    const char* GetStateName() const override { return "RETREATING"; }
};