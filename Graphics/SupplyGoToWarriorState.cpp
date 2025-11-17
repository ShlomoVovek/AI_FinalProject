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
    if (agent->IsSurviveMode() && agent->HasAmmo())
    {
        NPC* closerWarrior = agent->FindNearbyAllyNeedsResupply(true);
        if (closerWarrior != nullptr)
        {
            std::cout << "SupplyAgent (Survival Mode): INTERRUPT! Found *closer* warrior. Prioritizing.\n";

            auto it = std::find(agent->deliveryQueue.begin(), agent->deliveryQueue.end(), closerWarrior);
            if (it != agent->deliveryQueue.end())
            {
                agent->deliveryQueue.erase(it);
            }
            agent->deliveryQueue.push_front(closerWarrior);

            agent->SetState(new SupplyGoToWarriorState());
            return;
        }
    }

    NPC* targetWarrior = agent->GetDeliveryTarget();

    if (targetWarrior == nullptr || !targetWarrior->IsAlive())
    {
        if (!agent->deliveryQueue.empty())
        {
            agent->deliveryQueue.pop_front();
        }

        // Check if there are more targets
        if (agent->GetDeliveryTarget() != nullptr)
        {
            std::cout << "SupplyAgent: Next target in queue. Recalculating path.\n";
            pathCalculated = false; // Recalculate for new target
            return;
        }
        else
        {
            std::cout << "SupplyAgent: No more targets. Returning to Wait.\n";
            agent->SetState(new SupplyWaitState());
            return;
        }
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