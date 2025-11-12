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
    if (agent->NeedsSelfHeal())
    {
        std::cout << "Medic interrupted GO_TO_TARGET mission for self-heal!\n";
        agent->SetPatientTarget(agent);
        agent->SetState(new MedicHealingState());
        return;
    }
    NPC* patient = agent->GetPatientTarget();

    if (!patient || !patient->IsAlive())
    {
        std::cout << "Medic: Patient is dead or invalid. Returning to idle.\n";
        agent->SetPatientTarget(nullptr);
        agent->SetState(new MedicIdleState());
        return;
    }

    // Check if patient still needs healing
    if (patient->GetHealth() >= MAX_HP)
    {
        std::cout << "Medic: Patient is fully healed. Returning to idle.\n";
        agent->SetPatientTarget(nullptr);
        agent->SetState(new MedicIdleState());
        return;
    }

    // UPDATE: Use patient's CURRENT location, not the cached one
    Point targetLoc = patient->GetLocation();

    // Check if we're already close enough to heal
    Point medicLoc = agent->GetLocation();
    double dist = Distance(medicLoc, targetLoc);

    if (dist < 2.0)
    {
        // We're close enough - start healing
        std::cout << "Medic arrived at target. Starting to heal.\n";
        agent->SetState(new MedicHealingState());
        return;
    }

    // Need to recalculate path if patient moved significantly
    if (!pathCalculated || agent->currentPath.empty())
    {
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
                std::cerr << "Medic: Path to target not found!\n";
                agent->SetPatientTarget(nullptr);
                agent->SetState(new MedicIdleState());
                return;
            }
        }
        else
        {
            std::cerr << "Medic: Cannot calculate path to patient!\n";
            agent->SetPatientTarget(nullptr);
            agent->SetState(new MedicIdleState());
            return;
        }
    }

    // Continue moving
    if (agent->IsMoving())
    {
        agent->CalculatePathAndMove();

        // Periodically check if patient moved too far
        if (agent->currentPath.empty())
        {
            // Reached the target location but patient might have moved
            Point currentPatientLoc = patient->GetLocation();
            double distanceToPatient = Distance(medicLoc, currentPatientLoc);

            if (distanceToPatient >= 2.0)
            {
                // Patient moved, recalculate path
                std::cout << "Medic: Patient moved. Recalculating path.\n";
                pathCalculated = false;
            }
            else
            {
                // We're at the destination and patient is nearby
                agent->isMoving = false;
            }
        }
    }
    else
    {
        // Not moving but patient is nearby - start healing
        if (dist < 2.0)
        {
            std::cout << "Medic arrived at target. Starting to heal.\n";
            agent->SetState(new MedicHealingState());
        }
        else
        {
            // Need to recalculate path
            pathCalculated = false;
        }
    }
}

void MedicGoToTargetState::OnExit(Medic* agent)
{
    agent->isMoving = false; // Stop movement
    std::cout << "Medic exiting GO_TO_TARGET state.\n";
}