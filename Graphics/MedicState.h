#pragma once

class Medic; // Forward declaration

// Base class for all Medic states
class MedicState
{
public:
    virtual ~MedicState() = default;

    // Called once when entering this state
    virtual void OnEnter(Medic* agent) = 0;

    // Called every frame while in this state
    virtual void Execute(Medic* agent) = 0;

    // Called once when exiting this state
    virtual void OnExit(Medic* agent) = 0;

    // Returns the name of the state (for debugging)
    virtual const char* GetStateName() const = 0;
};