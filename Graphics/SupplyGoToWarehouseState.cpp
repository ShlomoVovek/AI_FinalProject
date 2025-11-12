#include "SupplyGoToWarehouseState.h"
#include "SupplyGoToWarriorState.h"
#include "SupplyAgent.h"
#include "SupplyIdleState.h"
#include "SupplyWaitState.h"
#include <iostream>
#include <algorithm>

void SupplyGoToWarehouseState::OnEnter(SupplyAgent* agent)
{
    std::cout << "SupplyAgent (Team " << (agent->GetTeam() == TEAM_RED ? "RED" : "BLUE")
        << ") entering GO_TO_WAREHOUSE state\n";
    pathCalculated = false;
    agent->isMoving = false; // Reset movement
    agent->ClearPath(); // Clear old path
}

void SupplyGoToWarehouseState::Execute(SupplyAgent* agent)
{
    // 1. חישוב נתיב אם לא חושב עדיין
    if (!pathCalculated)
    {
        Point warehousePos = agent->FindNearestWarehouse();

        std::cout << "SupplyAgent calculating path to warehouse at ("
            << warehousePos.x << ", " << warehousePos.y << ")\n";

        if (agent->FindAStarPath(warehousePos, agent->GetViewMap()))
        {
            std::cout << "Path to warehouse found! Steps: "
                << agent->GetCurrentPath().size() << "\n";
            pathCalculated = true;

            if (!agent->currentPath.empty())
            {
                agent->SetDirection(agent->currentPath.front());
                agent->isMoving = true;
            }
        }
        else
        {
            std::cout << "ERROR: No path to warehouse found! Returning to IDLE.\n";
            agent->SetState(new SupplyIdleState());
            return;
        }
    }

    // 2. תנועה אם יש נתיב ועדיין זז
    if (agent->IsMoving() || agent->HasPath())
    {
        agent->CalculatePathAndMove();

        // אם CalculatePathAndMove סיימה את התנועה בפריימ הזה, 
        // agent->isMoving יהיה false, והבלוק יסתיים. 
        // נחכה לפריימ הבא כדי לבדוק הגעה.
        if (agent->IsMoving())
        {
            return; // ממשיך לזוז, נחכה לפריימ הבא
        }
    }

    // 3. הגענו לכאן רק אם:
    //    א. pathCalculated = true
    //    ב. agent->IsMoving() = false
    //    ג. agent->HasPath() = false (הנתיב הסתיים ב-CalculatePathAndMove)

    // 4. בדיקת הגעה למחסן
    Point currentLoc = agent->GetLocation();
    Point warehousePos = agent->FindNearestWarehouse();
    double dist = Distance(currentLoc, warehousePos);

    if (dist <= 1.5) // הגעה מוצלחת
    {
        std::cout << "SupplyAgent reached warehouse, picking up ammo\n";
        agent->PickupAmmo();

        if (agent->GetDeliveryTarget() != nullptr)
        {
            agent->SetState(new SupplyGoToWarriorState());
        }
        else
        {
            agent->SetState(new SupplyWaitState());
        }
    }
    else
    {
        // תיקון: אם הנתיב נגמר לפני ההגעה, נסה לחשב נתיב מחדש
        std::cout << "SupplyAgent: Movement finished but did not reach warehouse (Dist: "
            << dist << "). Recalculating path.\n";

        // איפוס pathCalculated כדי שיחשב מחדש בפריים הבא
        pathCalculated = false;

        // אין צורך לשנות מצב, המצב הנוכחי יטפל בחישוב מחדש בפריים הבא
    }
}

void SupplyGoToWarehouseState::OnExit(SupplyAgent* agent)
{
    agent->isMoving = false; // Ensure movement stops
    std::cout << "SupplyAgent exiting GO_TO_WAREHOUSE state\n";
}