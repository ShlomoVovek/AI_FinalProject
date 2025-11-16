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
    // 1. טיפול במצב קריטי (נשאר כפי שהוא)
    if (commander->GetHealth() < CRITICAL_HP)
    {
        std::cout << "Commander critically wounded (Health: "
            << commander->GetHealth() << "), HOLDING POSITION for medic\n";
        commander->ClearPath();
        framesToWait = 100; // ממתין לחובש
    }
    else if (!commander->HasPath() && framesToWait <= 0)
    {
        // 2. חישוב נתיב יזום אם אין נתיב והמפקד לא מחכה
        Point strategicPoint = commander->FindSafePosition(); // נשתמש במיקום בטוח כברירת מחדל

        std::cout << "Commander initiating self-reposition to: ("
            << strategicPoint.x << ", " << strategicPoint.y << ")\n";

        if (commander->FindAStarPath(strategicPoint, commander->GetComabinedViewMap()))
        {
            std::cout << "Reposition path calculated. Moving.\n";
            // לא נשנה את framesToWait כדי לאפשר תנועה
        }
        else
        {
            std::cout << "Reposition path not found, going back to Analyzing.\n";
            commander->ClearPath();
            commander->SetState(new CommanderAnalyzingState());
            return; // יציאה מיידית אם לא נמצא נתיב
        }
    }

    if (commander->HasPath())
    {
        commander->MoveToTarget();
    }
    framesToWait--;

    if ((!commander->HasPath() && framesToWait <= 0) || (framesToWait <= 0 && commander->GetHealth() > 40))
    {
        if (commander->GetHealth() > 40)
        {
            commander->allSpottedEnemies.clear();
            commander->SetPlannedCommand(CMD_NONE, commander->GetLocation());
            std::cout << "Commander reposition complete, returning to command\n";
            commander->SetState(new CommanderAnalyzingState());
        }
        else
        {
            std::cout << "Commander still healing (Health: " << commander->GetHealth()
                << "), HOLDING POSITION\n";
            commander->ClearPath();
            framesToWait = 60;
        }
    }
}
void CommanderRepositioningState::OnExit(Commander* commander)
{
    std::cout << "Commander exiting REPOSITIONING state\n";
}