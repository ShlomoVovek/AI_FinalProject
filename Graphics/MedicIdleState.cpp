#include "MedicIdleState.h"
#include "Medic.h"
#include <iostream>

void MedicIdleState::OnEnter(Medic* agent)
{
    std::cout << "Medic entering IDLE state.\n";
    agent->isMoving = false; // Ensure medic stops moving
}

void MedicIdleState::Execute(Medic* agent)
{
    if (agent->NeedsSelfHeal())
    {
        std::cout << "Medic is injured. Initiating self-heal.\n";
        agent->SetState(new MedicHealingState());
        return;
    }
    
    if (agent->IsSurviveMode())
    {
        NPC* nearbyPatient = agent->FindNearbyInjuredAlly();
        if (nearbyPatient != nullptr)
        {
            // בדוק אם הוא כבר בתור
            auto it = std::find(agent->patientsQueue.begin(), agent->patientsQueue.end(), nearbyPatient);
            if (it == agent->patientsQueue.end())
            {
                std::cout << "Medic (Survival Mode): Found nearby patient. Adding to front of queue.\n";
                agent->patientsQueue.push_front(nearbyPatient);
            }
           
        }
    }

    if (!agent->patientsQueue.empty())
    {
        std::cout << "Medic has patient assignment. Moving to heal.\n";
        agent->SetState(new MedicGoToTargetState());
        return;
    }
}

void MedicIdleState::OnExit(Medic* agent)
{
    // std::cout << "Medic exiting IDLE state.\n";
}