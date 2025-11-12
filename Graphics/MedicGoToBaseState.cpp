#include "MedicGoToBaseState.h"
#include "Medic.h"
#include "MedicGoToTargetState.h" // The next state
#include "MedicIdleState.h"     // Fallback state
#include <iostream>

void MedicGoToBaseState::OnEnter(Medic* agent)
{
    std::cout << "Medic entering GO_TO_BASE state.\n";
    pathCalculated = false;
}

void MedicGoToBaseState::Execute(Medic* agent)
{
    if (agent->NeedsSelfHeal())
    {
        std::cout << "Medic interrupted GO_TO_BASE mission for self-heal!\n";
        agent->SetPatientTarget(agent);
        agent->SetState(new MedicHealingState());
        return;
    }
    if (!pathCalculated)
    {
        Point baseLoc = agent->GetBaseLocation();
        if (agent->FindAStarPath(baseLoc, (const double*)agent->GetViewMap()))
        {
            if (!agent->currentPath.empty())
            {
                agent->SetDirection(agent->currentPath.front());
                agent->isMoving = true;
                pathCalculated = true;
            }
            else
            {
                // Can't find path to base, go back to idle
                std::cerr << "Medic: Path to base not found!\n";
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
    else // Arrived at destination (Base)
    {
        std::cout << "Medic arrived at base. Now moving to target.\n";
        // TODO: Simulate picking up medicine

        // Now transition to GoToTarget state
        agent->SetState(new MedicGoToTargetState());
    }
}

void MedicGoToBaseState::OnExit(Medic* agent)
{
    agent->isMoving = false; // Stop movement when exiting state
    std::cout << "Medic exiting GO_TO_BASE state.\n";
}