#include "SupplyAgent.h"
#include "Definition.h"
#include <iostream>
#include <queue>
#include <map>
#include <algorithm>
#include <cmath>

extern const int MSX;
extern const int MSY;

SupplyAgent::SupplyAgent(int x, int y, TeamColor t) :
    NPC(x, y, t, SUPPLIER), currentState(S_IDLE), carriedAmmo(0), deliveryTarget(nullptr)
{
    targetWarehouse = GetBaseLocation();
}

Point SupplyAgent::GetBaseLocation() const // TODO: change to random location search by BFS
{
	if (this->team == TEAM_RED)
		return { 5, 5 };
	else
		return { MSX - 5, MSY - 5 };
}

void SupplyAgent::DoSomeWork(const double* pMap)
{

}

void SupplyAgent::CalculatePathAndMove()
{

}

// NPC methods
void SupplyAgent::ReportSighting(NpcType enemyType, Point enemyLoc)
{

}
void SupplyAgent::ReportLowAmmo(NPC* warrior)
{

}
void SupplyAgent::ReportInjury(NPC* injuredSoldier)
{

}
void SupplyAgent::ExecuteCommand(int commandCode, Point target)
{

}













