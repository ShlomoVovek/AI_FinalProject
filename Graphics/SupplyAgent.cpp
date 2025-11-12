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
    int BASE = this->GetTeam() == TEAM_BLUE ? RED_BASE : BLUE_BASE;
    Point p = { -1,-1 };

    if (this->GetTeam() == TEAM_RED)
    {
        for (int i = 3; i < MSX - 3; i++)
            for (int j = 3; j < MSY - 3; j++)
            {
                if (viewMap[i][j] == BASE)
                {
                    p.x = i;
                    p.y = j;
                    return p;
                }
            }
    }
    else // is blue
    {
        for (int i = MSX - 3; i > 0.5 * MSX; i--)
            for (int j = 3; j < MSY - 3 ; j++)
            {
                if (viewMap[i][j] == BASE)
                {
                    p.x = i;
                    p.y = j;
                    return p;
                }
            }
    }
    // default: never executed
    return p;
}

Point SupplyAgent::FindNearestWarehouse() const
{
    // TODO: use more sofisticated algorithm
    // Use GetBaseLocation for now
    return GetBaseLocation();
}

void SupplyAgent::PickupAmmo()
{
    std::cout << "Supply Agent: Loading supplies at warehouse.\n";
    cargoAmmo = MAX_AMMO;
    cargoGrenades = MAX_GRENADE; 
}

void SupplyAgent::DeliverAmmo()
{
    if ((cargoAmmo > 0 || cargoGrenades > 0) && deliveryTarget != nullptr && deliveryTarget->IsAlive())
    {
        Warrior* targetWarrior = dynamic_cast<Warrior*>(deliveryTarget);

        if (targetWarrior)
        {
            std::cout << "SupplyAgent delivering supplies to warrior\n";

            targetWarrior->Resupply(cargoAmmo, cargoGrenades);

            cargoAmmo = 0;
            cargoGrenades = 0;
        }
        deliveryTarget = nullptr;
    }
}

bool SupplyAgent::IsIdle() const
{
    return (dynamic_cast<SupplyIdleState*>(currentState) != nullptr);
}

void SupplyAgent::AssignSupplyMission(NPC* warrior)
{
    if (IsIdle() && warrior != nullptr && warrior->IsAlive())
    {
        deliveryTarget = warrior; 
        std::cout << "Supply Agent: Mission accepted. Going to warehouse.\n";

        SetState(new SupplyGoToWarehouseState());
    }
}

void SupplyAgent::DoSomeWork(const double* pMap)
{
    if (!IsAlive())
        return;

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
    }

    MoveToTarget();

    if (!isMoving)
    {
        currentPath.pop_front();

        if (!currentPath.empty())
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

    switch (commandCode)
    {
    case CMD_RETREAT:
        // ... (נסיגה למחסן הקרוב היא הפעולה הבטוחה ביותר עבור סוכן אספקה)
        targetLocation = FindNearestWarehouse();
        SetState(new SupplyGoToWarehouseState()); // המחסן הוא יעד בטוח
        break;

    case CMD_RESUPPLY: // כאשר המפקד שולח פקודת אספקה
    {
        // אם הסוכן כבר נושא תחמושת, הוא צריך ללכת ישירות ללוחם (אם מוגדר)
        if (cargoAmmo > 0)
        {
            // המפקד אמור לשלוח את המטרה דרך AssignSupplyMission, לא דרך ExecuteCommand
            // כאן אנחנו פשוט נכנסים למצב GoToWarehouse כדי לאסוף.
            SetState(new SupplyGoToWarehouseState());
        }
        else
        {
            // אם הסוכן ריק, הוא צריך ללכת לבסיס כדי לאסוף.
            SetState(new SupplyGoToWarehouseState());
        }
    }
    break;

    default:
        // ברירת מחדל לאידל או לוגיקת טיפול בשגיאות
        SetState(new SupplyIdleState());
        break;
    }
}













