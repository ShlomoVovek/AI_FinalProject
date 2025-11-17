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
        double distance = Distance(myPos, enemyPos);

        // 1. Check GRENADE "sweet spot" (long range, outside rifle range)
        if (distance > RIFLE_RANGE && distance <= GRENADE_RANGE && warrior->GetGrenades() > 0)
        {
            warrior->ThrowGrenade(enemyPos);
            std::cout << "Warrior throwing grenade at long-range enemy\n";
            framesSinceLastShot = 0;
        }
        // 2. Check RIFLE range
        else if (distance <= RIFLE_RANGE && warrior->CanShootAt(enemyPos))
        {
            if (framesSinceLastShot >= SHOOT_COOLDOWN)
            {
                if (warrior->GetAmmo() > 0)
                {
                    // Primary: Shoot rifle
                    warrior->Shoot(pEnemy);
                    framesSinceLastShot = 0;
                    std::cout << "Warrior shooting at enemy at ("
                        << enemyPos.x << ", " << enemyPos.y << ")\n";
                }
                else if (warrior->GetGrenades() > 0)
                {
                    // Fallback: Out of ammo, use grenade
                    warrior->ThrowGrenade(enemyPos);
                    std::cout << "Warrior out of ammo, throwing grenade\n";
                    framesSinceLastShot = 0;
                }
                else
                {
                    // Out of all weapons
                    warrior->RequestSupply();
                }
            }
        }
        // Report sighting
        warrior->ReportSighting(pEnemy->GetType(), enemyPos);
    }

    // 3. Check if reached attack position
    if (warrior->GetCurrentPath().empty() && !warrior->IsMoving())
    {
        pEnemy = warrior->ScanForEnemies();
        if (pEnemy == nullptr)
        {
            std::cout << "Warrior reached attack position, no enemy in sight. Returning to IDLE.\n";
            warrior->SetState(new WarriorIdleState());
            return; // Exit after changing state
        }
        else if (warrior->GetAmmo() == 0 && warrior->GetGrenades() == 0)
        {
            std::cout << "Warrior reached attack position, out of ammo. Retreating to safe spot.\n";
            warrior->RequestSupply();
            warrior->ExecuteCommand(CMD_RETREAT, warrior->GetLocation());
            return;
        }
        Point myPos = warrior->GetLocation();
        double dist = Distance(myPos, pEnemy->GetLocation());

        if (dist > RIFLE_RANGE && dist > GRENADE_RANGE)
        {
            std::cout << "Warrior at pos, enemy visible but out of range. Re-engaging.\n";
            warrior->ExecuteCommand(CMD_ATTACK, pEnemy->GetLocation());
            return;
        }
        std::cout << "Warrior reached attack position, engaging enemy\n";
        warrior->ClearPath();
    }

    // 4. Check critical health - might need to retreat
    if (warrior->GetHealth() < CRITICAL_HP) // Very low health
    {
        std::cout << "Warrior critically injured, requesting medic\n";
        warrior->ExecuteCommand(CMD_RETREAT, warrior->GetLocation());
        warrior->RequestMedic();
        return;
    }
}
void WarriorAttackingState::OnExit(Warrior* warrior)
{
    std::cout << "Warrior exiting ATTACKING state\n";
}