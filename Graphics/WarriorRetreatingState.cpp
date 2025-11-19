#include "WarriorRetreatingState.h"
#include "Warrior.h"
#include "WarriorIdleState.h"
#include <iostream>

void WarriorRetreatingState::OnEnter(Warrior* warrior)
{
    std::cout << "Warrior (Team " << (warrior->GetTeam() == TEAM_RED ? "RED" : "BLUE")
        << ") entered RETREATING state.\n";
}

void WarriorRetreatingState::Execute(Warrior* warrior)
{
    if (!warrior->IsMoving() && warrior->GetCurrentPath().empty())
    {
        std::cout << "Warrior reached safe position, setting state to IDLE.\n";
        warrior->SetState(new WarriorIdleState());
        return;
    }
    warrior->CalculatePathAndMove();

    NPC* pEnemy = warrior->ScanForEnemies();
    if (pEnemy && warrior->CanShootAt(pEnemy->GetLocation()))
    {
        warrior->Shoot(pEnemy);
    }
}

void WarriorRetreatingState::OnExit(Warrior* warrior)
{
    std::cout << "Warrior exiting RETREATING state\n";
}

