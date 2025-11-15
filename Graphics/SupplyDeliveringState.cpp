#include "SupplyDeliveringState.h"
#include "SupplyIdleState.h"
#include "SupplyWaitState.h"
#include "SupplyAgent.h"
#include "Definition.h"
#include "SupplyGoToWarriorState.h"
#include "SupplyGoToWarehouseState.h"
#include <iostream>

void SupplyDeliveringState::OnEnter(SupplyAgent* agent)
{
    std::cout << "SupplyAgent entering Delivering state.\n";
}

void SupplyDeliveringState::Execute(SupplyAgent* agent)
{
    NPC* targetWarrior = agent->GetDeliveryTarget();

    if (targetWarrior == nullptr || !targetWarrior->IsAlive())
    {
        std::cout << "SupplyAgent: Target is invalid or dead. Returning to Idle.\n";
        agent->SetState(new SupplyWaitState());
        return;
    }
   
    Point agentLoc = agent->GetLocation();
    Point warriorLoc = targetWarrior->GetLocation();

    double dist = Distance(agentLoc, warriorLoc);
    const double DELIVERY_RANGE = 2.5;
    
    if (dist <= DELIVERY_RANGE)
    {
        std::cout << "SupplyAgent is delivering ammo.\n";
        agent->DeliverAmmo();

        // 1. בדיקה האם יש משימה נוספת בתור
        if (agent->GetDeliveryTarget() != nullptr)
        {
            // אם יש, הוא יחזור למחסן (או יחכה אם זה כבר קרה)
            std::cout << "SupplyAgent: New target in queue. Going back to warehouse.\n";
            // המצב GoToWarehouseState מטפל אוטומטית במציאת המחסן הקרוב
            agent->SetState(new SupplyGoToWarehouseState());
        }
        else
        {
            // 2. אם אין משימה, הוא עובר למצב המתנה/בטוח
            std::cout << "SupplyAgent: Delivery complete. Moving to safe spot to wait.\n";
            agent->SetState(new SupplyWaitState());
        }
    }
    else
    {
        std::cout << "Warrior moved. SupplyAgent going back to GoToWarrior.\n"; 
        agent->SetState(new SupplyGoToWarriorState()); 
    }
}

void SupplyDeliveringState::OnExit(SupplyAgent* agent)
{
    std::cout << "SupplyAgent exiting DELIVERING state\n";
}