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
    while (agent->GetDeliveryTarget() != nullptr)
    {
        NPC* target = agent->GetDeliveryTarget();

        if (target->IsAlive())
        {
            break; // Found valid target
        }
        else
        {
            std::cout << "SupplyAgent: Removing dead warrior from queue in WAIT state.\n";
            agent->deliveryQueue.pop_front();
        }
    }

    // Now check if there's a valid delivery target
    if (agent->GetDeliveryTarget() != nullptr)
    {
        std::cout << "SupplyAgent: New delivery target found. Moving to warrior.\n";
        agent->SetState(new SupplyGoToWarriorState());
    }
    else
    {
        if (agent->IsSurviveMode() && agent->HasAmmo())
        {
            NPC* nearbyWarrior = agent->FindNearbyAllyNeedsResupply(false);
            if (nearbyWarrior != nullptr)
            {
                std::cout << "SupplyAgent (Survival Mode): Found nearby warrior. Adding to queue.\n";
                agent->deliveryQueue.push_back(nearbyWarrior);
                agent->SetState(new SupplyGoToWarriorState());
                return;
            }
        }

        // Stay in safe position
        agent->ClearPath();
        agent->isMoving = false;
    }
}

void SupplyWaitState::OnExit(SupplyAgent* agent)
{
    std::cout << "SupplyAgent exiting WAIT_SAFE state.\n";
}