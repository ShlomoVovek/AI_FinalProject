#include "SupplyIdleState.h"
#include "SupplyAgent.h"
#include <iostream>

void SupplyIdleState::OnEnter(SupplyAgent* agent)
{
    std::cout << "SupplyAgent (Team " << (agent->GetTeam() == TEAM_RED ? "RED" : "BLUE")
        << ") entering IDLE state - waiting for resupply orders\n";
}

void SupplyIdleState::Execute(SupplyAgent* agent)
{
    // Just wait for ExecuteCommand to be called by Commander
    // When CMD_RESUPPLY is received, ExecuteCommand will transition to GoToWarehouse
}

void SupplyIdleState::OnExit(SupplyAgent* agent)
{
    std::cout << "SupplyAgent exiting IDLE state\n";
}