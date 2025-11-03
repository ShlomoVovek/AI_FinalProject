#include "SupplyGoToWarriorState.h"
#include "SupplyDeliveringState.h"
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
    // 1. Get target warrior location
    Point targetPos = agent->GetTargetLocation();

    // 2. Calculate path on first frame
    if (!pathCalculated)
    {
        std::cout << "SupplyAgent calculating path to warrior at ("
            << targetPos.x << ", " << targetPos.y << ")\n";

        if (agent->FindAStarPath(targetPos, agent->GetViewMap()))
        {
            std::cout << "Path to warrior found! Steps: "
                << agent->GetCurrentPath().size() << "\n";
            pathCalculated = true;
        }
        else
        {
            std::cout << "ERROR: No path to warrior found!\n";
            return;
        }
    }

    // 3. Move along path
    agent->CalculatePathAndMove();

    // 4. Check if reached warrior (within 2 cells)
    Point currentPos = agent->GetLocation();
    double dist = ManhattanDistance(currentPos, targetPos);

    if (dist <= 2.0)
    {
        std::cout << "SupplyAgent reached warrior!\n";

        // Transition to delivering
        agent->SetState(new SupplyDeliveringState());
    }
}

void SupplyGoToWarriorState::OnExit(SupplyAgent* agent)
{
    std::cout << "SupplyAgent exiting GO_TO_WARRIOR state\n";
}