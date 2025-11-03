#pragma once

class Warrior;

class WarriorState
{
public:
    virtual ~WarriorState() = default;

    virtual void OnEnter(Warrior* warrior) = 0;
    virtual void Execute(Warrior* warrior) = 0;
    virtual void OnExit(Warrior* warrior) = 0;
    virtual const char* GetStateName() const = 0;
};