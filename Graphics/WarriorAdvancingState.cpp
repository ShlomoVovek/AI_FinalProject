#include "WarriorAdvancingState.h"
#include "WarriorIdleState.h"
#include "Warrior.h"
#include <iostream>

void WarriorAdvancingState::OnEnter(Warrior* warrior)
{
    std::cout << "Warrior entering ADVANCING state\n";
}

void WarriorAdvancingState::Execute(Warrior* warrior)
{
    // 1. Move along path
    warrior->CalculatePathAndMove();

    // 2. Check for enemies while moving
    NPC* pEnemy = warrior->ScanForEnemies();
    if (pEnemy != nullptr)
    {
        Point enemyPos = pEnemy->GetLocation();
        warrior->ReportSighting(WARRIOR, enemyPos);
    }

    // 3. Check if reached destination
    if (warrior->GetCurrentPath().empty() && !warrior->IsMoving())
    {
        std::cout << "Warrior reached destination, returning to IDLE\n";
        warrior->SetState(new WarriorIdleState());
    }

    // 4. Check health/ammo status
    if (warrior->GetHealth() < CRITICAL_HP)
    {
        warrior->RequestMedic();
    }

    if (warrior->GetAmmo() < CRITICAL_AMMO)
    {
        warrior->RequestSupply();
    }
}

void WarriorAdvancingState::OnExit(Warrior* warrior)
{
    std::cout << "Warrior exiting ADVANCING state\n";
}