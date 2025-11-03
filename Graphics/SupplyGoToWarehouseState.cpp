#include "SupplyGoToWarehouseState.h"
#include "SupplyGoToWarriorState.h"
#include "SupplyAgent.h"
#include <iostream>

void SupplyGoToWarehouseState::OnEnter(SupplyAgent* agent)
{
    std::cout << "SupplyAgent (Team " << (agent->GetTeam() == TEAM_RED ? "RED" : "BLUE")
        << ") entering GO_TO_WAREHOUSE state\n";
    pathCalculated = false;
    agent->hasAmmo = false;
}

void SupplyGoToWarehouseState::Execute(SupplyAgent* agent)
{
    // 1. Calculate path to warehouse on first frame
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
        }
        else
        {
            std::cout << "ERROR: No path to warehouse found!\n";
            // Stay in this state and try again next frame
            return;
        }
    }

    // 2. Move along path
    if (agent->HasPath())
    {
        agent->CalculatePathAndMove();

        // 3. Check if reached warehouse
        Point currentLoc = agent->GetLocation();
        double dist = std::min(Distance(currentLoc, agent->targetWarehouse[0]),
                               Distance(currentLoc, agent->targetWarehouse[1]));

        if (dist <= 1.5) // Close enough to warehouse
        {
            std::cout << "SupplyAgent reached warehouse, picking up ammo\n";
            agent->PickupAmmo();

            // Transition to next state
            agent->SetState(new SupplyGoToWarriorState());
        }
    }
}

void SupplyGoToWarehouseState::OnExit(SupplyAgent* agent)
{
    std::cout << "SupplyAgent exiting GO_TO_WAREHOUSE state\n";
}