#include "Commander.h"
#include "CommanderAnalyzingState.h"
#include <algorithm>
#include <cmath>
#include <iostream>

Commander::Commander(int x, int y, TeamColor t) :
	NPC(x, y, t, COMMANDER),
	currentState(nullptr),
	plannedCommand(CMD_NONE)

{
	for (int i = 0; i < MSX; ++i)
		for (int j = 0; j < MSY; ++j)
			combinedViewMap[i][j] = 0.0;

	currentState = new CommanderAnalyzingState();
	currentState->OnEnter(this);
}

Commander::~Commander()
{
	delete currentState;
}

void Commander::SetState(CommanderState* newState)
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

void Commander::CalculatePathAndMove()
{
	MoveToTarget(); // search logic
}

void Commander::ReportSighting(NpcType enemyType, Point EnemyLoc)
{
	Sighting s = { enemyType, EnemyLoc };
	allSpottedEnemies.push_back(s);
}

void Commander::UpdateCombinedViewMap()
{
	// 1. reset View Map
	for (int i = 0; i < MSX; ++i)
		for (int j = 0; j < MSY; ++j)
			combinedViewMap[i][j] = 0.0;
	
	// 2. sum all team members View Maps
	const double* memberViewMap = nullptr;
	for (NPC* member : teamMembers)
	{
		if (member->IsAlive() && member->GetType() != COMMANDER)
			memberViewMap = member->GetViewMap();

		for (int i = 0; i < MSX; i++)
			for (int j = 0; j < MSY; j++)
			{
				int index = i * MSY + j;
				if (combinedViewMap[i][j] != NULL &&
					memberViewMap[index] > combinedViewMap[i][j])
					combinedViewMap[i][j] = memberViewMap[index];
			}
	}
}

void Commander::IssueCommand(int commandCode, Point target)
{
	for (NPC* member : teamMembers)
	{
		if (member->IsAlive())
			member->ExecuteCommand(commandCode, target);
	}
}

void Commander::ReportInjury(NPC* injuredSoldier)
{
	bool alreadyInQueue = false;
	for (NPC* w : injuredSoldiers) {
		if (w == injuredSoldier) {
			alreadyInQueue = true;
			break;
		}
	}

	if (!alreadyInQueue && injuredSoldier->IsAlive()) {
		injuredSoldiers.push_back(injuredSoldier);
		std::cout << "Commander: Added soldier to medic queue.\n";
	}
}

void Commander::ReportLowAmmo(NPC* warrior)
{
	bool alreadyInQueue = false;
	for (NPC* w : resupplySoldiers)
	{
		if (w == warrior)
		{
			alreadyInQueue = true;
			break;
		}
	}

	if (!alreadyInQueue && warrior->IsAlive())
	{
		resupplySoldiers.push_back(warrior);
		std::cout << "Commander: Added " << warrior->GetType()
			<< " to supply request queue.\n";
	}
}

NPC* Commander::GetNextInjuredSoldier()
{
	if (injuredSoldiers.empty())
		return nullptr;

	NPC* soldier = injuredSoldiers.front();
	injuredSoldiers.erase(injuredSoldiers.begin());
	return soldier;
}

NPC* Commander::GetNextLowAmmoSoldier()
{
	if (resupplySoldiers.empty())
		return nullptr;

	NPC* soldier = resupplySoldiers.front();
	resupplySoldiers.erase(resupplySoldiers.begin());
	return soldier;
}

int Commander::GetWoundedCount() const
{
	int count = 0;
	for (NPC* member : teamMembers)
	{
		if (member->IsAlive() && !member->CanFight())
			count++;
	}
	return count;
}

Point Commander::FindSafePosition() const
{
	if (allSpottedEnemies.empty())
		return location;

	Point closeEnemy = allSpottedEnemies.front().point;

	double manhattanDist = ManhattanDistance(
		(int)location.x, (int)location.y,
		closeEnemy.x, closeEnemy.y
	);

	double dx = location.x - closeEnemy.x;
	double dy = location.y - closeEnemy.y;

	double euclideanDist = EuclideanDist(dx, dy);
	const double RETREAT_DISTANCE = 10.0;

	// go back
	Point safePos = { location.x + dx * 2, location.y + dy * 2 };

	if (euclideanDist > 0)
	{
		// נירמול וקטור ההתרחקות, והכפלה במרחק הנסיגה
		safePos.x = (int)(location.x + (dx / euclideanDist) * RETREAT_DISTANCE);
		safePos.y = (int)(location.y + (dy / euclideanDist) * RETREAT_DISTANCE);
	}
	else
	{
		safePos = location;
	}

	if (safePos.x < 1) safePos.x = 1;
	if (safePos.x >= MSX - 1) safePos.x = MSX - 2;
	if (safePos.y < 1) safePos.y = 1;
	if (safePos.y >= MSY - 1) safePos.y = MSY - 2;

	return safePos;
}

Point Commander::FindAttackPosition() const
{
	if (allSpottedEnemies.empty())
		return location;

	Point closestEnemy = allSpottedEnemies.front().point;

	double manhattanDist = ManhattanDistance(
		(int)location.x, (int)location.y,
		closestEnemy.x, closestEnemy.y
	);

	double dx = closestEnemy.x - location.x;
	double dy = closestEnemy.y - location.y;

	Point attackPos;
	if (manhattanDist > 15)
	{
		attackPos.x = (int)(closestEnemy.x - (dx / manhattanDist) * 15);
		attackPos.y = (int)(closestEnemy.y - (dy / manhattanDist) * 15);
	}
	else
	{
		attackPos = closestEnemy;
	}

	if (attackPos.x < 1) attackPos.x = 1;
	if (attackPos.x >= MSX - 1) attackPos.x = MSX - 2;
	if (attackPos.y < 1) attackPos.y = 1;
	if (attackPos.y >= MSY - 1) attackPos.y = MSY - 2;

	return attackPos;
}

void Commander::DoSomeWork(const double* pMap)
{
	if (!IsAlive())
		return;

	BuildViewMap(pMap);

	if (currentState)
	{
		currentState->Execute(this);
	}
}
