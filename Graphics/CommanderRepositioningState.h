#pragma once
#include "CommanderState.h"

class Commander;

// State: Commander moves to safer position (doesn't fight)
class CommanderRepositioningState : public CommanderState
{
private:
    int framesToWait; // Wait a few frames before returning to analyzing

public:
    CommanderRepositioningState() : framesToWait(30) {} // ~1 second at 30 FPS

    void OnEnter(Commander* commander) override;
    void Execute(Commander* commander) override;
    void OnExit(Commander* commander) override;
    const char* GetStateName() const override { return "REPOSITIONING"; }
};