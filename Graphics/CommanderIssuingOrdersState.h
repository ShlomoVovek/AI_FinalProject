#pragma once
#include "CommanderState.h"

class Commander;

// State: Commander issues commands to team
class CommanderIssuingOrdersState : public CommanderState
{
private:
    bool ordersIssued;

public:
    CommanderIssuingOrdersState() : ordersIssued(false) {}

    void OnEnter(Commander* commander) override;
    void Execute(Commander* commander) override;
    void OnExit(Commander* commander) override;
    const char* GetStateName() const override { return "ISSUING_ORDERS"; }
};