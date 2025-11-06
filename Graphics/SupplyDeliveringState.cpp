#include "SupplyDeliveringState.h"
#include "SupplyIdleState.h"
#include "SupplyAgent.h"
#include "Definition.h"
#include "SupplyGoToWarriorState.h"
#include <iostream>

void SupplyDeliveringState::OnEnter(SupplyAgent* agent)
{
    std::cout << "SupplyAgent entering Delivering state.\n";
    agent->isMoving = false;
    agent->currentPath.clear();
}

void SupplyDeliveringState::Execute(SupplyAgent* agent)
{
    Point agentLoc = agent->GetLocation();
    Point warriorLoc = agent->GetTargetLocation(); //

    double dist = Distance(agentLoc, warriorLoc);

    if (dist < 2.0)
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