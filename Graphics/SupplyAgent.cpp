#include "SupplyAgent.h"
#include "SupplyIdleState.h"
#include "SupplyWaitState.h"
#include "SupplyGoToWarehouseState.h"
#include "Definition.h"
#include <iostream>
#include <queue>
#include <map>
#include <algorithm>
#include <cmath>

extern const int MSX;
extern const int MSY;

SupplyAgent::SupplyAgent(int x, int y, TeamColor t) :
	NPC(x, y, t, SUPPLIER), currentState(nullptr), hasAmmo(false)
{
    targetWarehouse[0] = { -1, -1 };
    targetWarehouse[1] = { -1, -1 };

	// Start in Idle state
    currentState = new SupplyIdleState();
    currentState->OnEnter(this); 
}

SupplyAgent::~SupplyAgent()
{
	delete currentState;
}

void SupplyAgent::SetState(SupplyAgentState* newState)
{
    if (currentState)
    {
        currentState->OnExit(this);
        delete currentState;
    }

    currentState = newState;

    if (currentState)
    {
        currentState->OnEnter(this);
    }
}

// TODO: change to random location search by BFS
Point SupplyAgent::GetBaseLocation() const
{
    int BASE = this->GetTeam() == TEAM_RED ? RED_BASE : BLUE_BASE;

    // Search entire visible map
    for (int i = 0; i < MSX; i++)
    {
        for (int j = 0; j < MSY; j++)
        {
            if (viewMap[i][j] == BASE)
            {
                return { i, j };
            }
        }
    }

    // Not found in current view
    return { -1, -1 };
}

Point SupplyAgent::FindNearestWarehouse() const
{
    // First, check if we already have a cached warehouse location
    if (targetWarehouse[0].x >= 0 && targetWarehouse[0].x < MSX &&
        targetWarehouse[0].y >= 0 && targetWarehouse[0].y < MSY)
    {
        std::cout << "Using cached warehouse at (" << targetWarehouse[0].x
            << ", " << targetWarehouse[0].y << ")\n";
        return targetWarehouse[0];
    }

    // Try to find warehouse in current view
    Point warehouse = GetBaseLocation();

    if (warehouse.x >= 0 && warehouse.x < MSX &&
        warehouse.y >= 0 && warehouse.y < MSY)
    {
        // Cache it for future use
        const_cast<SupplyAgent*>(this)->targetWarehouse[0] = warehouse;
        std::cout << "Found and cached warehouse at (" << warehouse.x
            << ", " << warehouse.y << ")\n";
        return warehouse;
    }

    // Fallback: use team's starting area as warehouse location
    std::cout << "WARNING: Warehouse not found, using fallback location\n";
    if (this->GetTeam() == TEAM_RED)
    {
        warehouse = { 8, MSY / 2 };  // Left side base area
    }
    else
    {
        warehouse = { MSX - 8, MSY / 2 };  // Right side base area
    }

    // Cache fallback location
    const_cast<SupplyAgent*>(this)->targetWarehouse[0] = warehouse;
    return warehouse;
}

void SupplyAgent::PickupAmmo()
{
    std::cout << "Supply Agent: Loading supplies at warehouse.\n";
    cargoAmmo = MAX_AMMO;
    cargoGrenades = MAX_GRENADE; 
    hasAmmo = true;
}

void SupplyAgent::DeliverAmmo()
{
    // נשתמש ב-GetDeliveryTarget() כדי לקבל את הלוחם הנוכחי (הראשון בתור)
    NPC* targetWarrior = GetDeliveryTarget();

    // 1. בדיקה האם יש משימה נוכחית והאם הסוכן נושא תחמושת
    if ((cargoAmmo > 0 || cargoGrenades > 0) && targetWarrior != nullptr && targetWarrior->IsAlive())
    {
        Warrior* warriorToResupply = dynamic_cast<Warrior*>(targetWarrior);

        if (warriorToResupply)
        {
            std::cout << "SupplyAgent delivering supplies to warrior\n";

            // 2. אספקה
            warriorToResupply->Resupply(cargoAmmo, cargoGrenades);

            // 3. איפוס מטען הסוכן
            cargoAmmo = 0;
            cargoGrenades = 0;
            hasAmmo = false;
        }

        // 4. *** הסרת הלוחם מהתור ***
        deliveryQueue.pop_front();
    }
    // אם אין מטען או שהלוחם לא קיים/מת, נסיר רק את הלוחם מהתור (אם יש)
    else if (targetWarrior != nullptr)
    {
        // אם הסוכן הגיע אבל הלוחם מת או משהו השתבש, פשוט ננקה אותו מהתור
        deliveryQueue.pop_front();
    }
}

bool SupplyAgent::IsIdle() const
{
    return (dynamic_cast<SupplyIdleState*>(currentState) != nullptr);
}

void SupplyAgent::AssignSupplyMission(NPC* warrior)
{
    if (warrior != nullptr && warrior->IsAlive())
    {
        // 1. הוסף לתור
        deliveryQueue.push_back(warrior);

        // 2. אם הסוכן במצב סרק, התחל את המשימה
        if (dynamic_cast<SupplyIdleState*>(currentState) != nullptr ||
            dynamic_cast<SupplyWaitState*>(currentState) != nullptr)
        {
            std::cout << "Supply Agent: Mission accepted. Going to warehouse.\n";
            // קבע את targetLocation למיקום הנוכחי של הלוחם הראשון בתור
            targetLocation = deliveryQueue.front()->GetLocation();
            SetState(new SupplyGoToWarehouseState());
        }
    }
}

void SupplyAgent::DoSomeWork(const double* pMap)
{
    if (!IsAlive())
        return;

    DecayThreats();
    BuildViewMap(pMap);

    // Execute current state
    if (currentState)
    {
        currentState->Execute(this);
    }
}

void SupplyAgent::CalculatePathAndMove()
{
    if (currentPath.empty())
    {
        isMoving = false;
        return;
    }

    if (!isMoving)
    {
        SetDirection(currentPath.front());
        isMoving = true;
    }

    MoveToTarget();

    Point nextPoint = currentPath.front();
    if (Distance(location.x, location.y, nextPoint.x, nextPoint.y) < SPEED)
    {
        currentPath.pop_front();

        if (currentPath.empty())
        {
            isMoving = false;
        }
        else
        {
            SetDirection(currentPath.front());
        }
    }
}

// NPC methods
void SupplyAgent::ReportSighting(NpcType enemyType, Point enemyLoc)
{
    if (myCommander)
    {
        myCommander->ReportSighting(enemyType, enemyLoc);
    }
}
void SupplyAgent::ReportLowAmmo(NPC* warrior)
{
    return;
}
void SupplyAgent::ReportInjury(NPC* injuredSoldier)
{

}
void SupplyAgent::ExecuteCommand(int commandCode, Point target)
{
    if (!IsAlive()) return;

    if (!IsIdle() && dynamic_cast<SupplyWaitState*>(currentState) == nullptr)
    {
        if (commandCode == CMD_MOVE || commandCode == CMD_ATTACK)
        {
            std::cout << "SupplyAgent: Busy on supply mission, ignoring command "
                << commandCode << "\n";
            return;
        }
    }


    switch (commandCode)
    {
    case CMD_RETREAT:
        if (!deliveryQueue.empty())
        {
            std::cout << "SupplyAgent: RETREAT issued! Clearing delivery queue.\n";
            deliveryQueue.clear();
        }

        targetLocation = FindNearestWarehouse();
        SetState(new SupplyGoToWarehouseState());
        break;

    case CMD_RESUPPLY:
        SetState(new SupplyGoToWarehouseState());
        break;

    case CMD_MOVE:
        // Only respond if idle or waiting
        if (IsIdle() || dynamic_cast<SupplyWaitState*>(currentState) != nullptr)
        {
            targetLocation = target;
            if (FindAStarPath(target, GetViewMap()))
            {
                isMoving = true;
            }
        }
        break;

    default:
        break;
    }
}

void SupplyAgent::CleanDeliveryQueue()
{
    while (!deliveryQueue.empty())
    {
        NPC* front = deliveryQueue.front();
        if (front == nullptr || !front->IsAlive())
        {
            deliveryQueue.pop_front();
        }
        else
        {
            break; // First valid warrior found
        }
    }
}

NPC* SupplyAgent::FindNearbyAllyNeedsResupply(bool excludeCurrentTarget) const
{
    const double SURVIVAL_RESUPPLY_RADIUS = 5.0;

    if (npcList == nullptr) return nullptr;

    Point myLoc = GetLocation();
    NPC* currentTarget = excludeCurrentTarget ? GetDeliveryTarget() : nullptr;

    for (NPC* ally : *npcList)
    {
        // 1. בדיקות בסיסיות (לא אני, מהקבוצה שלי, חי, בטווח)
        if (ally == this ||
            ally->GetTeam() != this->GetTeam() ||
            !ally->IsAlive() ||
            Distance(myLoc.x, myLoc.y, ally->GetLocation().x, ally->GetLocation().y) > SURVIVAL_RESUPPLY_RADIUS)
        {
            continue;
        }

        // 2. בדיקה אם זה היעד הנוכחי שאנחנו רוצים להתעלם ממנו
        if (excludeCurrentTarget && ally == currentTarget)
        {
            continue;
        }

        // 3. בדיקה ספציפית: האם זה לוחם שצריך אספקה
        Warrior* warrior = dynamic_cast<Warrior*>(ally);
        if (warrior && warrior->IsNeedSupply())
        {
            return warrior; // מצאנו!
        }
    }

    return nullptr; // לא נמצא אף אחד
}










