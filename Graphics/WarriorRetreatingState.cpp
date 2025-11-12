#include "WarriorRetreatingState.h"
#include "Warrior.h"
#include "WarriorIdleState.h"
#include <iostream>

void WarriorRetreatingState::OnEnter(Warrior* warrior)
{
    // הנתיב כבר נמצא והוגדר ב-Warrior::ExecuteCommand
    std::cout << "Warrior (Team " << (warrior->GetTeam() == TEAM_RED ? "RED" : "BLUE")
        << ") entered RETREATING state.\n";
}

void WarriorRetreatingState::Execute(Warrior* warrior)
{
    // אם הנסיגה הסתיימה (הגיע ליעד הבטוח)
    if (!warrior->IsMoving() && warrior->GetCurrentPath().empty())
    {
        std::cout << "Warrior reached safe position, setting state to IDLE.\n";
        warrior->SetState(new WarriorIdleState());
        return;
    }

    // בצע תנועה
    warrior->MoveToTarget();

    // ירי הגנתי (Defensive Shooting) - ירי רק אם יש אויב בטווח ראייה/ירי.
    // הפקודה מאפשרת ירי, אך לא כמו במצב התקפה (כלומר, לא דורש מעבר למצב Attack).
    NPC* pEnemy = warrior->ScanForEnemies();
    if (pEnemy && warrior->CanShootAt(pEnemy->GetLocation()))
    {
        // אם נמצא אויב בטווח בזמן נסיגה, בצע ירי
        warrior->Shoot(pEnemy);
    }
}

void WarriorRetreatingState::OnExit(Warrior* warrior)
{
    std::cout << "Warrior exiting RETREATING state\n";
}

