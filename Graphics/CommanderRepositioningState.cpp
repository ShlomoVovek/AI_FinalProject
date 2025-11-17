#include "CommanderRepositioningState.h"
#include "CommanderAnalyzingState.h"
#include "Commander.h"
#include <iostream>

void CommanderRepositioningState::OnEnter(Commander* commander)
{
    if (isEmergencyRetreat || commander->GetHealth() < 50)
    {
        Point safePos = commander->FindSafePosition();

        std::cout << "Commander EMERGENCY RETREAT to position: ("
            << safePos.x << ", " << safePos.y << ") - Health: "
            << commander->GetHealth() << "\n";

        // Use A* to get there
        if (commander->FindAStarPath(safePos, commander->GetComabinedViewMap()))
        {
            std::cout << "Emergency path found! Retreating!\n";
        }
        else
        {
            std::cout << "No path found, staying in place!\n";
            commander->ClearPath();
        }
    }
    criticalHealthWaitCounter = 0;
}

void CommanderRepositioningState::Execute(Commander* commander)
{
    if (commander->GetHealth() < CRITICAL_HP)
    {
        criticalHealthWaitCounter++;

        if (criticalHealthWaitCounter > 100)
        {
            std::cout << "Commander: Medic timeout - returning to command despite low health\n";
            commander->SetPlannedCommand(CMD_NONE, commander->GetLocation());
            commander->SetState(new CommanderAnalyzingState());
            return;
        }

        framesToWait = 50;
        return;
    }
    criticalHealthWaitCounter = 0;

    if (isEmergencyRetreat && commander->HasPath())
    {
        commander->CalculatePathAndMove();

        if (!commander->HasPath())
        {
            std::cout << "Commander reached emergency safe position.\n";
            isEmergencyRetreat = false;
            framesToWait = 30;
        }
        return;
    }
    framesToWait--;

    if (framesToWait <= 0)
    {
        if (commander->GetHealth() <= 40)
        {
            std::cout << "Commander still healing (Health: " << commander->GetHealth()
                << "), HOLDING POSITION\n";
            commander->ClearPath();
            framesToWait = 30;
        }
        else
        {
            commander->SetPlannedCommand(CMD_NONE, commander->GetLocation());
            std::cout << "Commander reposition/wait complete, returning to command\n";
            commander->SetState(new CommanderAnalyzingState());
        }
    }
}
void CommanderRepositioningState::OnExit(Commander* commander)
{
    std::cout << "Commander exiting REPOSITIONING state\n";
}