#include "MedicHealingState.h"
#include "Medic.h"
#include "MedicIdleState.h" // Back to idle when done
#include <iostream>

void MedicHealingState::OnEnter(Medic* agent)
{
    std::cout << "Medic entering HEALING state.\n";
    healingTimer = 60; // Reset timer
    agent->isMoving = false;
}

void MedicHealingState::Execute(Medic* agent)
{
    if (healingTimer > 0)
    {
        healingTimer--;
    }
    else
    {
        // Finished healing
        std::cout << "Medic finished healing target.\n";

        // TODO: Find the actual patient NPC and heal them
        // NPC* patient = agent->GetPatientTarget();
        // if (patient)
        // {
        //     patient->heal(MAX_HP);
        // }
        // else
        // {
        //     // Need a way to find NPC at agent->GetFinalTargetLocation()
        // }

        // Go back to idle
        agent->SetState(new MedicIdleState());
    }
}

void MedicHealingState::OnExit(Medic* agent)
{
    std::cout << "Medic exiting HEALING state.\n";
    agent->SetPatientTarget(nullptr); // Clear target
}