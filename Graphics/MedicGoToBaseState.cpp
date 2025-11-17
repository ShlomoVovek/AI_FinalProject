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
        std::cout << "Medic interrupted GO_TO_BASE mission for self-heal!\\n";
        agent->SetState(new MedicHealingState());
        return;
    }

    if (agent->IsSurviveMode() && agent->HasMedicine())
    {
        NPC* nearbyPatient = agent->FindNearbyInjuredAlly();
        if (nearbyPatient != nullptr)
        {
            std::cout << "Medic (Survival Mode): INTERRUPT BASE! Found nearby patient. Going to heal.\n";

            // הוסף או העבר לראש התור
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
                std::cerr << "Medic: Path to base not found! Returning to idle.\\n";
                agent->SetState(new MedicIdleState());
                return;
            }
        }
    }

    if (agent->IsMoving())
    {
        agent->CalculatePathAndMove();
    }
    else
    {
        std::cout << "Medic arrived at base. Now checking for patients.\\n";
        agent->RestockMedicine();

        if (agent->GetNextPatient() != nullptr)
        {
            agent->SetState(new MedicGoToTargetState());
        }
        else
        {
            std::cout << "Medic: Queue is empty, returning to Idle.\\n";
            agent->SetState(new MedicIdleState());
        }
    }
}

void MedicGoToBaseState::OnExit(Medic* agent)
{
    agent->isMoving = false; // Stop movement when exiting state
    std::cout << "Medic exiting GO_TO_BASE state.\n";
}