#include "Medic.h"
#include "Definition.h"
#include <iostream>
#include <queue>
#include <map>
#include <algorithm>

extern const int MSX;
extern const int MSY;

Medic::Medic(int x, int y, TeamColor t) :
	NPC(x, y, t, MEDIC), patientTarget(nullptr)
{
	currentState = new MedicIdleState();
	currentState->OnEnter(this);
}

Medic::~Medic()
{
	delete currentState;
}

void Medic::SetState(MedicState* newState)
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

void Medic::DoSomeWork(const double* pMap)
{
	if (!IsAlive()) return;

	BuildViewMap(pMap);

	if (currentState)
	{
		currentState->Execute(this);
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
	// TODO: USE A*
	if (this->team == TEAM_RED)
		return { 5, 5 };
	else // TEAM_BLUE
		return { MSX - 5, MSY - 5 };
}

void Medic::ExecuteCommand(int CommandCode, Point target)
{
	if (!IsAlive()) return;

	switch (CommandCode)
	{
	case CMD_MOVE:
		// TODO: Implement a dedicated "Move" state if needed
		// For now, just go to idle
		SetState(new MedicIdleState());
		break;
	case CMD_HEAL:
	{
		// Only start healing if currently idle
		if (dynamic_cast<MedicIdleState*>(currentState))
		{
			finalTargetLocation = target; // This is the patient's location

			// TODO: Ideally, search npcList to find the actual NPC* patient
			// and store it in patientTarget.
			// For now, we only use the location (finalTargetLocation).

			std::cout << "Medic (Team " << (team == TEAM_RED ? "RED" : "BLUE")
				<< ") received HEAL command. Moving to base first.\n";

			// Transition to going to base
			SetState(new MedicGoToBaseState());
		}
		break;
	}
	default:
		// By default, go to idle
		SetState(new MedicIdleState());
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













