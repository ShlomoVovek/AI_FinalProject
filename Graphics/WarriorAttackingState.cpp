#include "WarriorAttackingState.h"
#include "WarriorIdleState.h"
#include "Warrior.h"
#include <iostream>
#include <cmath>

void WarriorAttackingState::OnEnter(Warrior* warrior)
{
    std::cout << "Warrior entering ATTACKING state\n";
    framesSinceLastShot = SHOOT_COOLDOWN; // allow immediate first shot
}

void WarriorAttackingState::Execute(Warrior* warrior)
{
    framesSinceLastShot++;

    // 1. Move towards attack position
    warrior->CalculatePathAndMove();

    // 2. Scan for enemies
    NPC* pEnemy = warrior->ScanForEnemies();
    if (pEnemy != nullptr)
    {
        Point enemyPos = pEnemy->GetLocation();

        // Calculate distance to enemy
        Point myPos = warrior->GetLocation();
        double dx = enemyPos.x - myPos.x;
        double dy = enemyPos.y - myPos.y;
        double distance = sqrt(dx * dx + dy * dy);

        if (distance <= RIFLE_RANGE && warrior->CanShootAt(enemyPos))
        {
            // Shoot if cooldown passed
            if (framesSinceLastShot >= SHOOT_COOLDOWN)
            {
                if (warrior->GetAmmo() > 0)
                {
                    warrior->Shoot(pEnemy);
                    framesSinceLastShot = 0;
                    std::cout << "Warrior shooting at enemy at ("
                        << enemyPos.x << ", " << enemyPos.y << ")\n";
                }
                else
                {
                    std::cout << "Warrior out of ammo!\n";
                    warrior->RequestSupply();
                }
            }
        }
        // Check grenade range
        else if (distance <= GRENADE_RANGE && warrior->GetGrenades() > 0)
        {
            warrior->ThrowGrenade(enemyPos);
            std::cout << "Warrior throwing grenade at enemy\n";
        }

        // Report sighting
        warrior->ReportSighting(WARRIOR, enemyPos);
    }

    // 3. Check if reached attack position
    if (warrior->GetCurrentPath().empty() && !warrior->IsMoving())
    {
        if (pEnemy == nullptr)
        {
            // We've arrived AND we don't see an enemy.
            // The enemy is dead or fled. Go back to IDLE.
            std::cout << "Warrior reached attack position, no enemy in sight. Returning to IDLE.\n";
            warrior->SetState(new WarriorIdleState());
            return; // Exit after changing state
        }
        else
        {
            // We've arrived AND we DO see an enemy.
            // Stay here and keep scanning/shooting.
            std::cout << "Warrior reached attack position, scanning for enemies\n";
        }
        std::cout << "Warrior reached attack position, scanning for enemies\n";
        // Stay in attack mode, continue scanning
    }

    // 4. Check critical health - might need to retreat
    if (warrior->GetHealth() < CRITICAL_HP * 0.5) // Very low health
    {
        std::cout << "Warrior critically injured, requesting medic\n";
        warrior->RequestMedic();
    }
}

void WarriorAttackingState::OnExit(Warrior* warrior)
{
    std::cout << "Warrior exiting ATTACKING state\n";
}