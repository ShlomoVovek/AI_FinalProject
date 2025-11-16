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
        if (!warrior->IsSurviveMode())
        {
            warrior->ReportSighting(pEnemy->GetType(), enemyPos);
        }
        else
        {
            if (warrior->CanShootAt(enemyPos))
            {
                std::cout << "Warrior spotted enemy while advancing in Survive Mode - Attacking!\n";
                warrior->ExecuteCommand(CMD_ATTACK, enemyPos);
                return;
            }
        }
    }

    // 3. Check if reached destination
    if (warrior->GetCurrentPath().empty() && !warrior->IsMoving())
    {
        std::cout << "Warrior reached destination, returning to IDLE\n";
        warrior->SetState(new WarriorIdleState());
    }

    // 4. Check health/ammo status
    if (warrior->GetHealth() < 0.5 * MAX_HP)
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