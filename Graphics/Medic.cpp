#include "Medic.h"
#include "Definition.h"
#include <iostream>
#include <queue>
#include <map>
#include <algorithm>

extern const int MSX;
extern const int MSY;

Medic::Medic(int x, int y, TeamColor t) :
	NPC(x, y, t, MEDIC), currentState(M_IDLE), patientTarget(nullptr)
{

}
void Medic::DoSomeWork(const double* pMap)
{
	if (!IsAlive()) return;

	switch (currentState)
	{
	case M_IDLE:
		break;

	case M_MOVING_TO_BASE:
		CalculatePathAndMove();

		// if medic arrived to base
		if (!this->isMoving && this->location.x == GetBaseLocation().x &&
			this->location.y == GetBaseLocation().y)
		{
			// go to injured soldier
			SetDirection(currentPath.front());
			this->isMoving = true;
			currentState = M_MOVING_TO_TARGET;
		}
		else
		{
			currentState = M_IDLE;
		}
		break;
	case M_MOVING_TO_TARGET:
		CalculatePathAndMove();
			break;

	case M_HEALING:
		break;

	default:
		break;
	}

}

void Medic::CalculatePathAndMove()
{
	if (this->isMoving && !currentPath.empty())
	{
		Point nextPoint = currentPath.front();
		SetDirection(nextPoint);

		if (Distance(location.x, location.y, nextPoint.x, nextPoint.y) < SPEED)
		{
			currentPath.pop_front();
			if (currentPath.empty())
			{
				this->isMoving = false;
			}
		}

		MoveToTarget();
	}
	else
	{
		this->isMoving = false;
	}
}

Point Medic::GetBaseLocation() const // TODO: change to random location search by BFS
{
	if (this->team == TEAM_RED)
		return { 5, 5 };
	else
		return { MSX - 5, MSY - 5 };
}

void Medic::ExecuteCommand(int CommandCode, Point target)
{
	switch (CommandCode)
	{
	case CMD_MOVE:
	case CMD_HEAL:
	{
		if (currentState == M_MOVING_TO_BASE || currentState == M_MOVING_TO_TARGET)
			return;

		Point baseLoc = GetBaseLocation();
		finalTargetLocation = target;

		if (FindAStarPath(baseLoc, (const double*)viewMap))
		{
			if(!currentPath.empty())
			{
				SetDirection(currentPath.front());
				this->isMoving = true;
				currentState = M_MOVING_TO_BASE;
			}
			else
			{
				currentState = M_RETREAT;
			}
		}
		break;
	// TODO: add M_RETREAT state
	// TODO: add M_RETREAT state when moving
	}
	default:
		currentState = M_IDLE;
		break;
	}
}

void Medic::ReportSighting(NpcType enemyType, Point enemyLoc)
{
	int x = 1;
}
void Medic::ReportLowAmmo(NPC* warrior)
{
	int x = 1;
}
void Medic::ReportInjury(NPC* injuredSoldier)
{
	int x = 1;
}













