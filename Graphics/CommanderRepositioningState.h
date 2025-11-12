#pragma once
#include "CommanderState.h"

class Commander;

// State: Commander moves to safer position (doesn't fight)
class CommanderRepositioningState : public CommanderState
{
private:
    int framesToWait; // Wait a few frames before returning to analyzing
    bool isEmergencyRetreat;

public:
    CommanderRepositioningState(bool emergency = false)
        : framesToWait(emergency ? 60 : 10),  // Wait longer if emergency
        isEmergencyRetreat(emergency)
    {
    }

    void OnEnter(Commander* commander) override;
    void Execute(Commander* commander) override;
    void OnExit(Commander* commander) override;
    const char* GetStateName() const override { return "REPOSITIONING"; }
};