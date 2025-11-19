#include "SupplyGoToWarehouseState.h"
#include "SupplyGoToWarriorState.h"
#include "SupplyAgent.h"
#include "SupplyIdleState.h"
#include "SupplyWaitState.h"
#include <iostream>
#include <algorithm>

void SupplyGoToWarehouseState::OnEnter(SupplyAgent* agent)
{
    Point warehouse = agent->FindNearestWarehouse();
    std::cout << "DEBUG: Nearest warehouse at (" << warehouse.x << ", " << warehouse.y << ")\n";
    
    std::cout << "SupplyAgent (Team " << (agent->GetTeam() == TEAM_RED ? "RED" : "BLUE")
        << ") entering GO_TO_WAREHOUSE state\n";
    pathCalculated = false;
    agent->isMoving = false; // Reset movement
    agent->ClearPath(); // Clear old path
}

void SupplyGoToWarehouseState::Execute(SupplyAgent* agent)
{
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
    if (agent->IsMoving() || agent->HasPath())
    {
        agent->CalculatePathAndMove();

        if (agent->IsMoving())
        {
            return;
        }
    }

    Point currentLoc = agent->GetLocation();
    Point warehousePos = agent->FindNearestWarehouse();
    double dist = Distance(currentLoc, warehousePos);

    if (dist <= 1.5)
    {
        std::cout << "SupplyAgent reached warehouse, picking up ammo\n";
        agent->PickupAmmo();

        if (agent->GetDeliveryTarget() != nullptr)
        {
            std::cout << "SupplyAgent: Restocked and moving to warrior.\\n";
            agent->SetTargetLocation(agent->GetDeliveryTarget()->GetLocation());
            agent->SetState(new SupplyGoToWarriorState());
        }
        else
        {
            std::cout << "SupplyAgent: Restocked, queue is empty. Returning to safe Idle.\\n";
            agent->SetState(new SupplyWaitState());
        }
    }
    else
    {
        std::cout << "SupplyAgent: Movement finished but did not reach warehouse (Dist: "
            << dist << "). Recalculating path.\n";

        pathCalculated = false;
    }
}

void SupplyGoToWarehouseState::OnExit(SupplyAgent* agent)
{
    agent->isMoving = false; // Ensure movement stops
    std::cout << "SupplyAgent exiting GO_TO_WAREHOUSE state\n";
}