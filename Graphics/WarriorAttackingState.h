#pragma once
#include "WarriorState.h"

class WarriorAttackingState : public WarriorState
{
private:
    int framesSinceLastShot;
    const int SHOOT_COOLDOWN = 15; // frames between shots

public:
    WarriorAttackingState() : framesSinceLastShot(0) {}

    void OnEnter(Warrior* warrior) override;
    void Execute(Warrior* warrior) override;
    void OnExit(Warrior* warrior) override;
    const char* GetStateName() const override { return "ATTACKING"; }
};
