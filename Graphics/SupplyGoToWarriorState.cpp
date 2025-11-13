#include "SupplyGoToWarriorState.h"
#include "SupplyDeliveringState.h"
#include "SupplyWaitState.h"
#include "SupplyAgent.h"
#include <iostream>

void SupplyGoToWarriorState::OnEnter(SupplyAgent* agent)
{
    std::cout << "SupplyAgent (Team " << (agent->GetTeam() == TEAM_RED ? "RED" : "BLUE")
        << ") entering GO_TO_WARRIOR state\n";
    pathCalculated = false;
}

void SupplyGoToWarriorState::Execute(SupplyAgent* agent)
{
    NPC* targetWarrior = agent->GetDeliveryTarget();

    // בדיקה בסיסית, כפי שקיים גם במצב Delivering (רצוי להוסיף כאן בדיקה זו אם היא חסרה)
    if (targetWarrior == nullptr || !targetWarrior->IsAlive())
    {
        std::cout << "SupplyAgent: Target is invalid or dead on GoToWarrior. Returning to Wait.\n";
        agent->SetState(new SupplyWaitState());
        return;
    }

    // ** הוספת שורה זו: שומר את המיקום הנוכחי של הלוחם **
    Point newTargetPos = targetWarrior->GetLocation();
    const double RECALCULATE_THRESHOLD = 4.0;
    double distanceMoved = ManhattanDistance(newTargetPos, agent->GetTargetLocation());

    if (!pathCalculated || distanceMoved > RECALCULATE_THRESHOLD)
    {
        // ** עדכון מיקום היעד של הסוכן למיקום הנוכחי של הלוחם **
        agent->SetTargetLocation(newTargetPos);

        std::cout << "SupplyAgent calculating path to warrior at ("
            << newTargetPos.x << ", " << newTargetPos.y << ")\n";

        // **שימוש ב-A* עם המיקום המעודכן**
        if (agent->FindAStarPath(newTargetPos, agent->GetViewMap()))
        {
            std::cout << "Path to warrior found! Steps: "
                << agent->GetCurrentPath().size() << "\n";
            pathCalculated = true;
        }
        else
        {
            std::cout << "ERROR: No path to warrior found! Returning to Wait.\n";
            agent->SetState(new SupplyWaitState());
            return;
        }
    }

    // 3. תנועה אם יש נתיב
    agent->CalculatePathAndMove();

    // 4. בדיקת הגעה למיקום הלוחם הנוכחי
    Point currentPos = agent->GetLocation();
    // **בדיקת הגעה מול המיקום הנוכחי של הלוחם**
    double dist = ManhattanDistance(currentPos, newTargetPos);

    if (dist <= 2.0)
    {
        // ... (מעבר ל-DeliveringState)
        agent->SetState(new SupplyDeliveringState());
    }
    // הערה: אם הלוחם זז והסוכן עדיין בתנועה, נמשיך ב-Execute בפריים הבא.
    // אם הלוחם זז משמעותית, הוא יתגלה בבדיקת התנאי הראשונה בפריים הבא.
}

void SupplyGoToWarriorState::OnExit(SupplyAgent* agent)
{
    std::cout << "SupplyAgent exiting GO_TO_WARRIOR state\n";
}