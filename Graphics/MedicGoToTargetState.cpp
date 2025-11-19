#include "MedicGoToTargetState.h"
#include "Medic.h"
#include "MedicHealingState.h" // The next state
#include "MedicIdleState.h"  // Fallback state
#include <iostream>

void MedicGoToTargetState::OnEnter(Medic* agent)
{
    std::cout << "Medic entering GO_TO_TARGET state.\\n";
    pathCalculated = false;

    NPC* patient = agent->GetNextPatient();

    if (!patient || !patient->IsAlive() || patient->GetHealth() >= MAX_HP)
    {
        if (patient)
        {
            std::cout << "Medic: Patient is dead, fully healed, or invalid on GO_TO_TARGET entry. Removing from queue.\\n";
            agent->RemoveCurrentPatient();
        }
        else
        {
            std::cout << "Medic: No patient in queue on GO_TO_TARGET entry. Returning to idle.\\n";
        }

        if (agent->GetNextPatient() == nullptr)
        {
            agent->SetState(new MedicIdleState());
        }
        return;
    }
}

void MedicGoToTargetState::Execute(Medic* agent)
{
    if (agent->NeedsSelfHeal())
    {
        std::cout << "Medic interrupted GO_TO_TARGET mission for self-heal!\\n";
        agent->SetState(new MedicHealingState());
        return;
    }

    if (agent->IsSurviveMode() && agent->HasMedicine())
    {
        NPC* nearbyPatient = agent->FindNearbyInjuredAlly(true);

        if (nearbyPatient != nullptr)
        {
            std::cout << "Medic (Survival Mode): INTERRUPT TARGET! Found *closer* patient. Prioritizing.\n";

            auto it = std::find(agent->patientsQueue.begin(), agent->patientsQueue.end(), nearbyPatient);
            if (it != agent->patientsQueue.end())
            {
                agent->patientsQueue.remove(nearbyPatient);
            }
            agent->patientsQueue.push_front(nearbyPatient);

            agent->SetState(new MedicGoToTargetState());
            return;
        }
    }

    NPC* patient = agent->GetNextPatient();
    Point medicLoc = agent->GetLocation();

    if (!patient || !patient->IsAlive() || patient->GetHealth() >= MAX_HP)
    {
        if (patient)
        {
            std::cout << "Medic: Patient is dead, fully healed, or invalid. Removing from queue.\\n";
            agent->RemoveCurrentPatient();
        }

        if (agent->GetNextPatient() == nullptr)
        {
            std::cout << "Medic: No more patients. Returning to idle.\\n";
            agent->SetState(new MedicIdleState());
        }
        else
        {
            std::cout << "Medic: Moving to next patient in queue.\\n";
            pathCalculated = false;
            agent->currentPath.clear();
        }
        return;
    }

    Point targetLoc = patient->GetLocation();
    double dist = Distance(medicLoc.x, medicLoc.y, targetLoc.x, targetLoc.y);

 
    if (dist <= 2.0)
    {
        std::cout << "Medic arrived at target. Starting to heal.\\n";
        agent->SetState(new MedicHealingState());
        return;
    }

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
        }
        else
        {
            std::cerr << "Medic: Path to target not found! Removing patient and checking next.\\n";
            agent->RemoveCurrentPatient();
            if (agent->GetNextPatient() == nullptr)
                agent->SetState(new MedicIdleState());
            else
                pathCalculated = false;
            return;
        }
    }

    if (agent->IsMoving())
    {
        agent->CalculatePathAndMove();

        if (agent->currentPath.empty())
        {
            Point currentPatientLoc = patient->GetLocation();
            double distanceToPatient = Distance(medicLoc.x, medicLoc.y, currentPatientLoc.x, currentPatientLoc.y);

            if (distanceToPatient >= 2.0)
            {
                std::cout << "Medic: Patient moved. Recalculating path.\\n";
                pathCalculated = false;
            }
            else
            {
                agent->isMoving = false;
            }
        }
    }
}


void MedicGoToTargetState::OnExit(Medic* agent)
{
    agent->isMoving = false; // Stop movement
    std::cout << "Medic exiting GO_TO_TARGET state.\n";
}