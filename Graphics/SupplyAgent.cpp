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













