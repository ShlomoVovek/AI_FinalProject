#pragma once

/// <summary>
/// Set the base interface for all Commander states. Each of those inherits from this class.
/// </summary>

class Commander;

class CommanderState
{
public:
    virtual ~CommanderState() = default;

    // Called once when entering this state
    virtual void OnEnter(Commander* commander) = 0;

    // Called every frame while in this state
    virtual void Execute(Commander* commander) = 0;

    // Called once when exiting this state
    virtual void OnExit(Commander* commander) = 0;

    // Returns the name of the state (for debugging)
    virtual const char* GetStateName() const = 0;
};