#include "SupplyAgent.h"
#include "SupplyIdleState.h"
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
    currentState = new SupplyGoToWarehouseState();
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
    Point p = { -1,-1 };
    int BASE = this->GetTeam() == TEAM_BLUE ? -5 : -6;

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
        // ... מקרים קיימים (CMD_NONE, CMD_MOVE, CMD_RESUPPLY)

    case CMD_RETREAT: // הפיכת CMD_RETREAT לפעולת ברירת המחדל של סוכן האספקה לנסיגה
        // נסיגה למחסן הקרוב היא הפעולה הבטוחה ביותר עבור סוכן אספקה
        targetLocation = FindNearestWarehouse(); // מציאת המחסן הקרוב כנקודת יעד בטוחה
        SetState(new SupplyGoToWarehouseState());
        break;

    default:
        // ברירת מחדל לאידל או לוגיקת טיפול בשגיאות
        SetState(new SupplyIdleState());
        break;
    }
}













