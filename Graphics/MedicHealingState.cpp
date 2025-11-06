#include "MedicHealingState.h"
#include "Medic.h"
#include "MedicIdleState.h" // Back to idle when done
#include <iostream>

void MedicHealingState::OnEnter(Medic* medic)
{
    std::cout << "Medic entering Healing state.\n";
    medic->isMoving = false;
    medic->currentPath.clear();
}

void MedicHealingState::Execute(Medic* medic)
{
    NPC* patient = medic->GetPatientTarget();

    if (!patient || !patient->IsAlive() || patient->GetHealth() >= MAX_HP)
    {
        std::cout << "Patient is gone or fully healed. Medic going idle.\n";
        medic->SetPatientTarget(nullptr);
        medic->SetState(new MedicIdleState());
        return;
    }

    Point medicLoc = medic->GetLocation();
    Point patientLoc = patient->GetLocation();
    double dist = Distance(medicLoc, patientLoc);
    if (dist < 2.0)
    {
        std::cout << "Medic is healing patient.\n";
        patient->heal(HEAL_AMOUNT_PER_TICK);

        if (patient->GetHealth() >= MAX_HP)
        {
            std::cout << "Patient fully healed. Medic going idle.\n";
            medic->SetPatientTarget(nullptr);
            medic->SetState(new MedicIdleState());
        }
    }
    else
    {
        std::cout << "Patient moved. Medic going back to GoToTarget.\n";
        medic->SetState(new MedicGoToTargetState());
    }
}

void MedicHealingState::OnExit(Medic* agent)
{
    std::cout << "Medic exiting HEALING state.\n";
    agent->SetPatientTarget(nullptr); // Clear target
}