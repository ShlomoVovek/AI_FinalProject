#include "MedicIdleState.h"
#include "Medic.h"
#include <iostream>

void MedicIdleState::OnEnter(Medic* agent)
{
    // std::cout << "Medic entering IDLE state.\n";
    agent->isMoving = false; // Ensure medic stops moving
}

void MedicIdleState::Execute(Medic* agent)
{
    // Do nothing, wait for a command or a report
}

void MedicIdleState::OnExit(Medic* agent)
{
    // std::cout << "Medic exiting IDLE state.\n";
}