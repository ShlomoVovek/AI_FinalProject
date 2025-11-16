#include "WarriorIdleState.h"
#include "Warrior.h"
#include <iostream>

void WarriorIdleState::OnEnter(Warrior* warrior)
{
    std::cout << "Warrior entering IDLE state\n";
    warrior->ClearPath();
}

void WarriorIdleState::Execute(Warrior* warrior)
{
    // Check health and ammo status
    if (warrior->GetHealth() < CRITICAL_HP && !warrior->HasRequestedMedic())
    {
        warrior->RequestMedic();
    }

    if (warrior->GetAmmo() < CRITICAL_AMMO && !warrior->HasRequestedSupply())
    {
        warrior->RequestSupply();
    }

    // Scan for enemies in view map
    Point enemyPos;
    NPC* pEnemy = warrior->ScanForEnemies();
    if (pEnemy != nullptr)
    {
        enemyPos = pEnemy->GetLocation();
        
        if (!warrior->IsSurviveMode())
        {
            warrior->ReportSighting(pEnemy->GetType(), enemyPos);
        }
        else
        {
            std::cout << "Warrior spotted enemy in IDLE (Survive Mode) - Attacking!\n";
            warrior->ExecuteCommand(CMD_ATTACK, enemyPos);
        }
    }
}

void WarriorIdleState::OnExit(Warrior* warrior)
{
    std::cout << "Warrior exiting IDLE state\n";
}