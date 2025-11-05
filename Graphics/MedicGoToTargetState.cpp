#include "MedicGoToTargetState.h"
#include "Medic.h"
#include "MedicHealingState.h" // The next state
#include "MedicIdleState.h"  // Fallback state
#include <iostream>

void MedicGoToTargetState::OnEnter(Medic* agent)
{
    std::cout << "Medic entering GO_TO_TARGET state.\n";
    pathCalculated = false;
}

void MedicGoToTargetState::Execute(Medic* agent)
{
    if (!pathCalculated)
    {
        Point targetLoc = agent->GetFinalTargetLocation();
        if (agent->FindAStarPath(targetLoc, (const double*)agent->GetViewMap()))
        {
            if (!agent->currentPath.empty())
            {
                agent->SetDirection(agent->currentPath.front());
                agent->isMoving = true;
                pathCalculated = true;
            }
            else
            {
                // Can't find path to target, go back to idle
                std::cerr << "Medic: Path to target not found!\n";
                agent->SetState(new MedicIdleState());
                return;
            }
        }
    }

    // Continue moving
    if (agent->IsMoving())
    {
        agent->CalculatePathAndMove();
    }
    else // Arrived at destination (Target)
    {
        std::cout << "Medic arrived at target. Starting to heal.\n";
        agent->SetState(new MedicHealingState());
    }
}

void MedicGoToTargetState::OnExit(Medic* agent)
{
    agent->isMoving = false; // Stop movement
    std::cout << "Medic exiting GO_TO_TARGET state.\n";
}