#include "Warrior.h"
#include "Definition.h"
#include "Commander.h"
#include <iostream>
#include <queue>
#include <map>
#include <algorithm>

Warrior::Warrior(int x, int y, TeamColor t) :
	NPC(x, y, t, WARRIOR), ammo(MAX_AMMO), isAttacking(false), currentState(W_IDLE), moveCooldown(0)
{
}

// TODO: build this method
Point Warrior::DetermineBestAttackPosition(Point enemyLoc)
{
	// Calculate a tactical position near the enemy
	double dx = enemyLoc.x - location.x;
	double dy = enemyLoc.y - location.y;
	double dist = Distance(location, enemyLoc);

	if (dist < 1.0) dist = 1.0;

	// Position at fighting range (10 units from enemy)
	Point attackPos;
	attackPos.x = (int)(location.x + (dx / dist) * (dist - 10));
	attackPos.y = (int)(location.y + (dy / dist) * (dist - 10));

	// Clamp to map boundaries
	attackPos.x = std::max(1, std::min(MSX - 2, attackPos.x));
	attackPos.y = std::max(1, std::min(MSY - 2, attackPos.y));

	std::cout << "Warrior determining attack position: ("
		<< attackPos.x << ", " << attackPos.y << ") towards enemy at ("
		<< enemyLoc.x << ", " << enemyLoc.y << ")\n";

	return attackPos;
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
}
void Warrior::Shoot(Point enemyLocation)
{
	if (ammo > 0)
	{
		ammo--;
		std::cout << "Warrior shooting! Ammo left: " << ammo << "\n";
	}
}
void Warrior::ThrowGrenade(Point enemyLocation)
{
	if (grenades > 0)
	{
		grenades--;
		std::cout << "Warrior throwing grenade!\n";
	}
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

	std::cout << "Warrior (Team " << (team == TEAM_RED ? "RED" : "BLUE")
		<< ") at (" << location.x << ", " << location.y
		<< ") received command: " << commandCode
		<< " target: (" << target.x << ", " << target.y << ")\n";

	// Don't re-execute same command with same target
	if ((commandCode == CMD_MOVE && currentState == W_ADVANCE) ||
		(commandCode == CMD_ATTACK && currentState == W_ATTACK) ||
		(commandCode == CMD_DEFEND && currentState == W_RETREAT))
	{
		// Check if target is same
		if (location.x == target.x && location.y == target.y)
		{
			currentState = W_IDLE;
			return;
		}
	}

	// Clear old path
	currentPath.clear();

	switch (commandCode)
	{
	case CMD_MOVE:
		currentState = W_ADVANCE;

		std::cout << "Warrior ADVANCING from (" << location.x << ", " << location.y
			<< ") to (" << target.x << ", " << target.y << ")\n";

		// CRITICAL FIX: Cast viewMap correctly as 1D array
		if (FindAStarPath(target, &viewMap[0][0]))
		{
			if (!currentPath.empty())
			{
				SetDirection(currentPath.front());
				std::cout << "Path found! Steps: " << currentPath.size() << "\n";
			}
			else
			{
				std::cout << "ERROR: Path found but empty!\n";
			}
		}
		else
		{
			std::cout << "ERROR: No path found to target!\n";
			currentState = W_IDLE;
		}
		break;

	case CMD_ATTACK:
	{
		currentState = W_ATTACK;
		currentAttackTarget = target;

		Point attackPos = DetermineBestAttackPosition(target);

		std::cout << "Warrior ATTACKING from (" << location.x << ", " << location.y
			<< ") moving to attack position (" << attackPos.x << ", " << attackPos.y << ")\n";

		// CRITICAL FIX: Cast viewMap correctly as 1D array
		if (FindAStarPath(attackPos, &viewMap[0][0]))
		{
			if (!currentPath.empty())
			{
				SetDirection(currentPath.front());
				std::cout << "Attack path found! Steps: " << currentPath.size() << "\n";
			}
			else
			{
				std::cout << "ERROR: Attack path found but empty!\n";
			}
		}
		else
		{
			std::cout << "ERROR: No attack path found!\n";
			currentState = W_IDLE;
		}
		break;
	}

	case CMD_DEFEND:
	{
		currentState = W_RETREAT;
		double searchRange = 50.0;

		std::cout << "Warrior RETREATING from (" << location.x << ", " << location.y << ")\n";

		// CRITICAL FIX: Cast viewMap correctly as 1D array
		Point safePos = FindClosestSafePosition(searchRange, &viewMap[0][0]);

		if (safePos.x != location.x || safePos.y != location.y)
		{
			std::cout << "Safe position found at (" << safePos.x << ", " << safePos.y << ")\n";

			// CRITICAL FIX: Cast viewMap correctly as 1D array
			if (FindAStarPath(safePos, &viewMap[0][0]))
			{
				if (!currentPath.empty())
				{
					SetDirection(currentPath.front());
					std::cout << "Retreat path found! Steps: " << currentPath.size() << "\n";
				}
			}
			else
			{
				std::cout << "ERROR: No retreat path found!\n";
				currentState = W_IDLE;
			}
		}
		else
		{
			std::cout << "Already at safe position\n";
			currentState = W_IDLE;
		}
		break;
	}

	default:
		std::cout << "Unknown command: " << commandCode << "\n";
		break;
	}
	/* add:
		supply
		injured
	*/

}
void Warrior::Resupply(int amount)
{

}

