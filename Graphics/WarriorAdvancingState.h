#pragma once
#include "WarriorState.h"

class WarriorAdvancingState : public WarriorState
{
public:
    void OnEnter(Warrior* warrior) override;
    void Execute(Warrior* warrior) override;
    void OnExit(Warrior* warrior) override;
    const char* GetStateName() const override { return "ADVANCING"; }
};