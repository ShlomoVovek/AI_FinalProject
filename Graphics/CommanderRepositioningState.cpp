#include "CommanderRepositioningState.h"
#include "CommanderAnalyzingState.h"
#include "Commander.h"
#include <iostream>

void CommanderRepositioningState::OnEnter(Commander* commander)
{
    if (isEmergencyRetreat || commander->GetHealth() < 50)
    {
        // EMERGENCY: Find position away from enemies
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
            commander->ClearPath();  // Stop moving if no path
        }
    }
}

void CommanderRepositioningState::Execute(Commander* commander)
{
    // CRITICAL: If health is critical, STOP MOVING to let medic catch up
    if (commander->GetHealth() < CRITICAL_HP)
    {
        std::cout << "Commander critically wounded (Health: "
            << commander->GetHealth() << "), HOLDING POSITION for medic\n";

        commander->ClearPath();  // STOP MOVING!
        framesToWait = 100;  // Wait longer for medic
    }
    else if (commander->HasPath())
    {
        // Only move if health is above critical threshold
        commander->MoveToTarget();
    }

    framesToWait--;

    // Only return to duty if health is reasonable AND not under fire
    if (framesToWait <= 0)
    {
        if (commander->GetHealth() > 40)
        {
            commander->allSpottedEnemies.clear();
            commander->SetPlannedCommand(CMD_NONE, commander->GetLocation());
            std::cout << "Commander recovered (Health: " << commander->GetHealth()
                << "), returning to command\n";
            commander->SetState(new CommanderAnalyzingState());
        }
        else
        {
            std::cout << "Commander still healing (Health: " << commander->GetHealth()
                << "), HOLDING POSITION\n";
            commander->ClearPath();  // STOP MOVING
            framesToWait = 60;  // Keep waiting
        }
    }
}

void CommanderRepositioningState::OnExit(Commander* commander)
{
    std::cout << "Commander exiting REPOSITIONING state\n";
}