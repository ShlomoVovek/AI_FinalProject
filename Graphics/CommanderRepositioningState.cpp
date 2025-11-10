#include "CommanderRepositioningState.h"
#include "CommanderAnalyzingState.h"
#include "Commander.h"
#include <iostream>

void CommanderRepositioningState::OnEnter(Commander* commander)
{
    commander->MoveToTarget();

    // If enemies are nearby, move to safe position
    if (commander->HasSpottedEnemies())
    {
        Point safePos = commander->FindSafePosition();
        commander->SetDirection(safePos);

        std::cout << "Commander repositioning to safe position: ("
            << safePos.x << ", " << safePos.y << ")\n";
    }
}

void CommanderRepositioningState::Execute(Commander* commander)
{
    // Move towards safe position (commander doesn't use pathfinding, just moves)
    commander->MoveToTarget();

    // Count down frames
    framesToWait--;

    // After waiting period, return to analyzing state
    if (framesToWait <= 0)
    {
        commander->allSpottedEnemies.clear();
        commander->SetPlannedCommand(CMD_NONE, commander->GetLocation());

        std::cout << "Commander cleared enemy sightings and planned command, returning to analyze fresh data\n";
        commander->SetState(new CommanderAnalyzingState());
    }
}

void CommanderRepositioningState::OnExit(Commander* commander)
{
    std::cout << "Commander exiting REPOSITIONING state\n";
}