#include "SupplyDeliveringState.h"
#include "SupplyIdleState.h"
#include "SupplyAgent.h"
#include <iostream>

void SupplyDeliveringState::OnEnter(SupplyAgent* agent)
{
    std::cout << "SupplyAgent (Team " << (agent->GetTeam() == TEAM_RED ? "RED" : "BLUE")
        << ") entering DELIVERING state - giving ammo to warrior\n";

    // Give ammo to warrior
    agent->DeliverAmmo();
}

void SupplyDeliveringState::Execute(SupplyAgent* agent)
{
    // Wait for delivery animation/timer
    deliveryTimer--;

    if (deliveryTimer <= 0)
    {
        std::cout << "SupplyAgent finished delivering ammo\n";

        // Return to idle state
        agent->SetState(new SupplyIdleState());
    }
}

void SupplyDeliveringState::OnExit(SupplyAgent* agent)
{
    std::cout << "SupplyAgent exiting DELIVERING state\n";
}