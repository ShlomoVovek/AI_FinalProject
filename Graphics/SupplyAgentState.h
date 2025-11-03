#pragma once

class SupplyAgent;

// Base class for all SupplyAgent states
class SupplyAgentState
{
public:
    virtual ~SupplyAgentState() = default;

    // Called once when entering this state
    virtual void OnEnter(SupplyAgent* agent) = 0;

    // Called every frame while in this state
    virtual void Execute(SupplyAgent* agent) = 0;

    // Called once when exiting this state
    virtual void OnExit(SupplyAgent* agent) = 0;

    // Returns the name of the state (for debugging)
    virtual const char* GetStateName() const = 0;
};