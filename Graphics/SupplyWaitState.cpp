#include "SupplyWaitState.h"
#include "SupplyAgent.h"
#include "SupplyGoToWarriorState.h"
#include <iostream>

void SupplyWaitState::OnEnter(SupplyAgent* agent)
{
    std::cout << "SupplyAgent entering WAIT_SAFE state - finding safe spot.\n";
    positionFound = false;
    agent->currentPath.clear();
    agent->isMoving = false;
}

void SupplyWaitState::Execute(SupplyAgent* agent)
{
    // 1. בדיקה האם המפקד נתן משימה חדשה בזמן ההמתנה
    if (agent->GetDeliveryTarget() != nullptr)
    {
        std::cout << "SupplyAgent: New delivery target found. Moving to warrior.\n";
        agent->SetState(new SupplyGoToWarriorState());
        return;
    }

    // 2. מציאת מיקום בטוח (פעם אחת)
    if (!positionFound)
    {
        // שימוש ב-BFS דרך IPathfinding::FindClosestSafePosition
        // טווח חיפוש של 20 יחידות
        Point safePos = agent->FindClosestSafePosition(20.0, agent->GetViewMap());

        if (safePos != agent->GetLocation())
        {
            // מציאת מסלול באמצעות A*
            if (agent->FindAStarPath(safePos, agent->GetViewMap()))
            {
                std::cout << "SupplyAgent: Moving to safe position ("
                    << safePos.x << ", " << safePos.y << ")\n";
                agent->isMoving = true;
                positionFound = true;
            }
            else
            {
                std::cout << "SupplyAgent: No path to safe spot found, waiting here.\n";
                positionFound = true; // כדי שלא ינסה למצוא שוב
            }
        }
        else
        {
            std::cout << "SupplyAgent: Already at a safe position, holding.\n";
            positionFound = true;
        }
    }

    // 3. המשך תנועה אם יש מסלול
    if (agent->IsMoving())
    {
        agent->CalculatePathAndMove();
    }
    else
    {
        // אם הגיע ליעד או לא יכול לזוז, נשאר במצב זה ומחכה לפקודה חדשה.
        agent->isMoving = false;
    }
}

void SupplyWaitState::OnExit(SupplyAgent* agent)
{
    std::cout << "SupplyAgent exiting WAIT_SAFE state.\n";
}