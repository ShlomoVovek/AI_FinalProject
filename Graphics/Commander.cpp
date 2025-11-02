#include "Commander.h"
#include <algorithm>
#include <cmath>
#include <iostream>

Commander::Commander(int x, int y, TeamColor t) :
	NPC(x, y, t, COMMANDER), currentTeamCommand(CMD_NONE)
{
	for (int i = 0; i < MSX; ++i)
		for (int j = 0; j < MSY; ++j)
			combinedViewMap[i][j] = 0.0;
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
				if (memberViewMap[index] > combinedViewMap[i][j])
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
	if (std::find(injuredSoldiers.begin(), injuredSoldiers.end(), injuredSoldier) == injuredSoldiers.end())
		injuredSoldiers.push_back(injuredSoldier);
}

void Commander::ReportLowAmmo(NPC* warrior)
{
	if (std::find(resupplySoldires.begin(), resupplySoldires.end(), warrior) == resupplySoldires.end())
		resupplySoldires.push_back(warrior);
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

	// go back
	Point safePos = { location.x + dx * 2, location.y + dy * 2 };
	safePos.x = std::max(1.0, std::min(static_cast<double>(MSX - 1), static_cast<double>(safePos.x)));
	safePos.y = std::max(1.0, std::min(static_cast<double>(MSY - 1), static_cast<double>(safePos.y)));

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
	if (!IsAlive()) return;

	BuildViewMap(pMap);

	// 1. sum members maps
	UpdateCombinedViewMap();
	
	// 2. handle ammo & injury reports
	if (!injuredSoldiers.empty())
	{
		// TODO: send only to medic, afterwards: reset to CMD_NONE
		NPC* target = injuredSoldiers.front();

		IssueCommand(CMD_HEAL, target->GetLocation());
		injuredSoldiers.erase(injuredSoldiers.begin());
		// return;
	}

	if (!resupplySoldires.empty()) 
	{
		// TODO: send only to SupplyAgent, afterwards: reset to CMD_NONE
		NPC* target = resupplySoldires.front();
		IssueCommand(CMD_RESUPPLY, target->GetLocation());

		resupplySoldires.erase(resupplySoldires.begin());
		// return;
	}

	// 3. attack or defend
	if (!allSpottedEnemies.empty())
	{
		int woundedCount = 0;
		for (NPC* member : teamMembers)
		{
			if (member->IsAlive() && !member->CanFight()) woundedCount++;
		}

		if (woundedCount >= teamMembers.size() / 2) // most are wounded
		{
			if (currentTeamCommand != CMD_DEFEND)
			{
				IssueCommand(CMD_DEFEND, location); // go for safe location
				std::cout << "Commander (Team " << (team == TEAM_RED ? "RED" : "BLUE") <<
					") ISSUED COMMAND: DEFEND (RETREAT)\n";

				currentTeamCommand = CMD_DEFEND;
				currentTeamTarget = location; // TODO:  <---earse this line
			}
		}
		else // most are not wounded
		{
			Point attackPos = FindAttackPosition();

			if (currentTeamCommand != CMD_ATTACK || (int)currentTeamTarget.x != (int)attackPos.x ||
				(int)currentTeamTarget.y != (int)attackPos.y)
			{
				IssueCommand(CMD_ATTACK, attackPos);
				std::cout << "Commander (Team " << (team == TEAM_RED ? "RED" : "BLUE") <<
					") ISSUED COMMAND: ATTACK Target: (" << (int)attackPos.x << ", " <<
					(int)attackPos.y << ")\n";

				currentTeamCommand = CMD_ATTACK;
				currentTeamTarget = attackPos;
			}
		}
	}
	// 4. no enemies spotted, change location
	else
	{
		if (currentTeamCommand != CMD_MOVE)
		{
			Point newTarget = { MSX / 2.0, MSY / 2.0 };
			IssueCommand(CMD_MOVE, newTarget);

			std::cout << "Commander (Team " << (team == TEAM_RED ? "RED" : "BLUE") <<
				") ISSUED COMMAND: MOVE to center (" << (int)newTarget.x << ", " <<
				(int)newTarget.y << ")\n";

			currentTeamCommand = CMD_MOVE;
			currentTeamTarget = newTarget;
		}
	}

	// 5. change commander position
	if (!allSpottedEnemies.empty())
	{
		Point safePos = FindSafePosition();
		SetDirection(safePos);
	}
	MoveToTarget();

	printf("%d\n", currentTeamCommand);
	printf("currentTeamTarget.x = %d\n", currentTeamTarget.x);
	printf("currentTeamTarget.y = %d\n", currentTeamTarget.y);
}
