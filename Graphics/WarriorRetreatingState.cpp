#include "WarriorRetreatingState.h"
#include "WarriorIdleState.h"
#include "Warrior.h"
#include <iostream>

void WarriorRetreatingState::OnEnter(Warrior* warrior)
{
    std::cout << "Warrior entering RETREATING state\n";
    safePositionFound = false;
}

void WarriorRetreatingState::Execute(Warrior* warrior)
{
    // 1. If haven't found safe position yet, find it
    if (!safePositionFound)
    {
        Point safePos = warrior->FindClosestSafePosition(50.0, warrior->GetViewMap());

        if (safePos.x != warrior->GetLocation().x ||
            safePos.y != warrior->GetLocation().y)
        {
            std::cout << "Warrior found safe position at ("
                << safePos.x << ", " << safePos.y << ")\n";

            if (warrior->FindAStarPath(safePos, warrior->GetViewMap()))
            {
                safePositionFound = true;
            }
        }
        else
        {
            std::cout << "Warrior already at safe position\n";
            warrior->SetState(new WarriorIdleState());
            return;
        }
    }

    // 2. Move towards safe position
    warrior->CalculatePathAndMove();

    // 3. Defensive shooting - shoot if enemy is close
    Point enemyPos;
    if (warrior->ScanForEnemies(enemyPos))
    {
        double distance = Distance(warrior->GetLocation(), enemyPos);

        // Only shoot if enemy is dangerously close
        if (distance <= RILFE_RANGE * 0.7 && warrior->GetAmmo() > 5)
        {
            if (warrior->CanShootAt(enemyPos))
            {
                warrior->Shoot(enemyPos);
                std::cout << "Warrior defensive shot while retreating\n";
            }
        }

        warrior->ReportSighting(WARRIOR, enemyPos);
    }

    // 4. Check if reached safe position
    if (warrior->GetCurrentPath().empty() && !warrior->IsMoving())
    {
        std::cout << "Warrior reached safe position\n";
        warrior->SetState(new WarriorIdleState());
    }
}

void WarriorRetreatingState::OnExit(Warrior* warrior)
{
    std::cout << "Warrior exiting RETREATING state\n";
}