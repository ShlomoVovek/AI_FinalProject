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

Point SupplyAgent::GetBaseLocation() const // TODO: change to random location search by BFS
{
	if (this->team == TEAM_RED)
		return { 5, 5 };
	else
		return { MSX - 5, MSY - 5 };
}

Point SupplyAgent::FindNearestWarehouse() const
{
    // Use GetBaseLocation for now
    return GetBaseLocation();
}

void SupplyAgent::PickupAmmo()
{
    hasAmmo = true;
    std::cout << "SupplyAgent picked up ammo from warehouse\n";
}

void SupplyAgent::DeliverAmmo()
{
    if (hasAmmo)
    {
        hasAmmo = false;
        std::cout << "SupplyAgent delivered ammo to warrior\n";

        // TODO: Actually give ammo to the warrior
        // Need to find the warrior NPC and call warrior->Resupply()
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

    std::cout << "SupplyAgent (Team " << (team == TEAM_RED ? "RED" : "BLUE")
        << ") received command: " << commandCode
        << " target: (" << target.x << ", " << target.y << ")\n";

    if (commandCode == CMD_RESUPPLY)
    {
        // Store target location (warrior who needs ammo)
        targetLocation = target;

        // Transition to going to warehouse
        SetState(new SupplyGoToWarehouseState());
    }
}













