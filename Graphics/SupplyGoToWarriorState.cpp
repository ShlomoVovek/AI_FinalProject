#include "SupplyGoToWarriorState.h"
#include "SupplyDeliveringState.h"
#include "SupplyWaitState.h"
#include "SupplyAgent.h"
#include <iostream>

void SupplyGoToWarriorState::OnEnter(SupplyAgent* agent)
{
    std::cout << "SupplyAgent (Team " << (agent->GetTeam() == TEAM_RED ? "RED" : "BLUE")
        << ") entering GO_TO_WARRIOR state\n";
    pathCalculated = false;
}

void SupplyGoToWarriorState::Execute(SupplyAgent* agent)
{
    if (agent->IsSurviveMode() && agent->HasAmmo())
    {
        NPC* closerWarrior = agent->FindNearbyAllyNeedsResupply(true);
        if (closerWarrior != nullptr)
        {
            std::cout << "SupplyAgent (Survival Mode): INTERRUPT! Found *closer* warrior. Prioritizing.\n";

            auto it = std::find(agent->deliveryQueue.begin(), agent->deliveryQueue.end(), closerWarrior);
            if (it != agent->deliveryQueue.end())
            {
                agent->deliveryQueue.erase(it);
            }
            agent->deliveryQueue.push_front(closerWarrior);

            agent->SetState(new SupplyGoToWarriorState());
            return;
        }
    }

    NPC* targetWarrior = agent->GetDeliveryTarget();

    if (targetWarrior == nullptr || !targetWarrior->IsAlive())
    {
        if (!agent->deliveryQueue.empty())
        {
            agent->deliveryQueue.pop_front();
        }

        // Check if there are more targets
        if (agent->GetDeliveryTarget() != nullptr)
        {
            std::cout << "SupplyAgent: Next target in queue. Recalculating path.\n";
            pathCalculated = false; // Recalculate for new target
            return;
        }
        else
        {
            std::cout << "SupplyAgent: No more targets. Returning to Wait.\n";
            agent->SetState(new SupplyWaitState());
            return;
        }
    }

    Point newTargetPos = targetWarrior->GetLocation();
    const double RECALCULATE_THRESHOLD = 4.0;
    double distanceMoved = ManhattanDistance(newTargetPos, agent->GetTargetLocation());

    if (!pathCalculated || distanceMoved > RECALCULATE_THRESHOLD)
    {
        agent->SetTargetLocation(newTargetPos);

        std::cout << "SupplyAgent calculating path to warrior at ("
            << newTargetPos.x << ", " << newTargetPos.y << ")\n";

        if (agent->FindAStarPath(newTargetPos, agent->GetViewMap()))
        {
            std::cout << "Path to warrior found! Steps: "
                << agent->GetCurrentPath().size() << "\n";
            pathCalculated = true;
        }
        else
        {
            std::cout << "ERROR: No path to warrior found! Returning to Wait.\n";
            agent->SetState(new SupplyWaitState());
            return;
        }
    }

    agent->CalculatePathAndMove();

    Point currentPos = agent->GetLocation();
    double dist = ManhattanDistance(currentPos, newTargetPos);

    if (dist <= 2.0)
    {
        agent->SetState(new SupplyDeliveringState());
    }
}

void SupplyGoToWarriorState::OnExit(SupplyAgent* agent)
{
    std::cout << "SupplyAgent exiting GO_TO_WARRIOR state\n";
}