#include "SupplyDeliveringState.h"
#include "SupplyIdleState.h"
#include "SupplyWaitState.h"
#include "SupplyAgent.h"
#include "Definition.h"
#include "SupplyGoToWarriorState.h"
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
    const double DELIVERY_RANGE = 2.0;

    if (dist <= DELIVERY_RANGE)
    {
        std::cout << "SupplyAgent is delivering ammo.\n"; 
        agent->DeliverAmmo(); 
        agent->SetState(new SupplyIdleState());
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