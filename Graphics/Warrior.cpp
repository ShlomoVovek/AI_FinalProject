#include "Warrior.h"
#include "Definition.h"
#include "Commander.h"
#include <iostream>
#include <queue>
#include <map>
#include <algorithm>

Warrior::Warrior(int x, int y, TeamColor t) :
	NPC(x, y, t, WARRIOR), ammo(MAX_AMMO), isAttacking(false), currentState(W_IDLE)
{
}

Point Warrior::DetermineBestAttackPosition(Point enemyLoc)
{
	Point point;
	point.x = 0;
	point.y = 0;
	return point;
}

void Warrior::CalculatePathAndMove()
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
	/*
	if (currentPath.empty())
	{
		isMoving = false;
		return;
	}
	
	// 1. set next step
	Point nextStep = currentPath.front();

	// 2. set direction
	SetDirection(nextStep);

	// 3. make a step
	MoveToTarget();

	// 4. check status
	if (!isMoving)
	{
		currentPath.pop_front();

		if (!currentPath.empty())
		{
			SetDirection(currentPath.front());
		}
	}

	// 5. 
	if (currentState == W_ATTACK)
	{
		// check ammo
		// check visibility
		// check range

		// shoot only if enemy is visible and in range
		if (currentState == W_RETREAT) // && enemy in range
		{
			// shoot() || ThrowGrenade()
		}
	}
	*/
}
void Warrior::Shoot(Point enemyLocaion)
{

}
void Warrior::ThrowGrenade(Point enemyLocaion)
{

}
void Warrior::DoSomeWork(const double* pMap)
{
	if (!IsAlive()) return;

	BuildViewMap(pMap);

	switch (currentState)
	{
	case W_IDLE:
		break;

	case W_ADVANCE:
	case W_ATTACK:
	case W_RETREAT:
	{
		CalculatePathAndMove();

		if (currentPath.empty() && !isMoving)
		{
			currentState = W_IDLE;
		}
		break;
	}

	default:
		break;
	}
}

void Warrior::ReportSighting(NpcType enemyType, Point enemyLoc)
{
	if (myCommander)
	{
		myCommander->ReportSighting(enemyType, enemyLoc);
	}
}
void Warrior::ReportLowAmmo(NPC* warrior)
{
	if (myCommander)
	{
		myCommander->ReportLowAmmo(warrior);
	}
}
void Warrior::ReportInjury(NPC* injuredSoldier)
{
	if (myCommander)
	{
		myCommander->ReportLowAmmo(injuredSoldier);
	}

}
void Warrior::ExecuteCommand(int commandCode, Point target)
{
	if (!IsAlive()) return;

	if ((commandCode == CMD_MOVE && currentState == W_ADVANCE) ||
		(commandCode == CMD_ATTACK && currentState == W_ATTACK) ||
		(commandCode == CMD_DEFEND && currentState == W_RETREAT))
	{
		return;
	}

	if (commandCode == CMD_MOVE && (int)location.x == (int)target.x &&
		(int)location.y == (int)target.y)
	{
		currentState = W_IDLE;
		return;
	}

	currentPath.clear();

	std::cout << "Warrior (Team " << (team == TEAM_RED ? "RED" : "BLUE") <<
		") at (" << (int)location.x << ", " << (int)location.y << ") Executing: ";

	switch (commandCode)
	{
	case CMD_MOVE:
		currentState = W_ADVANCE; // find the safest path forward with A*
		
		if (FindAStarPath(target, (const double*)viewMap) && !currentPath.empty())
		{
			SetDirection(currentPath.front());
		}
		std::cout << "ADVANCE to (" << (int)target.x << ", " << (int)target.y << ")\n";
		break;

	case CMD_ATTACK:
		currentState = W_ATTACK;
		currentAttackTarget = target;

		Point attackPos = DetermineBestAttackPosition(target);
		
		if (FindAStarPath(attackPos, (const double*)viewMap) && !currentPath.empty())
		{
			SetDirection(currentPath.front());
		}
		std::cout << "ATTACK Target: (" << (int)target.x << ", " << (int)target.y << ")\n";
		break;

	case CMD_DEFEND:
		currentState = W_RETREAT;
		double searchRange = 50.0;

		Point safePos = FindClosestSafePosition(searchRange, (const double*)viewMap);
		if (safePos.x != -1)
		{
			if (FindAStarPath(safePos, (const double*)viewMap) && !currentPath.empty())
			{
				SetDirection(currentPath.front());
			}
		}
		std::cout << "DEFEND at (" << (int)location.x << ", " << (int)location.y << ")\n";
		break;

	/* add:
		supply
		injured
	*/
	}
}
void Warrior::Resupply(int amount)
{

}

