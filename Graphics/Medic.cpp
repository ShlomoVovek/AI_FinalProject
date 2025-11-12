#include "Medic.h"
#include "Definition.h"
#include "Map.h"
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
{									 // TODO: return both bases
	int BASE = this->GetTeam() == TEAM_BLUE ? -5 : -6;
	Point p = {-1,-1};
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

void Medic::ExecuteCommand(int CommandCode, Point target)
{
	if (!IsAlive()) return;
	
	isMoving = false;

	switch (CommandCode)
	{
	case CMD_RETREAT:
		SetState(new MedicGoToBaseState());
		break;

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
			finalTargetLocation = target; // injured npc's location

			// Find the injured soldier at this location
			if (npcList != nullptr)
			{
				for (NPC* npc : *npcList)
				{
					if (npc->IsAlive() &&
						npc->GetTeam() == this->GetTeam() &&
						npc->GetLocation().x == target.x &&
						npc->GetLocation().y == target.y)
					{
						patientTarget = npc;
						break;
					}
				}
			}

			if (patientTarget != nullptr)
			{
				std::cout << "Medic (Team " << (team == TEAM_RED ? "RED" : "BLUE")
					<< ") received HEAL command. Moving to base first.\n";

				SetState(new MedicGoToBaseState());
			}
			else
			{
				std::cout << "Medic: Could not find injured soldier at target location!\n";
				SetState(new MedicIdleState());
			}
		}
		break;
	}
	default:
		SetState(new MedicGoToBaseState());
		break;
	}
}

bool Medic::IsIdle() const
{
	return dynamic_cast<MedicIdleState*>(currentState) != nullptr;
}

void Medic::AssignHealMission(NPC* injuredSoldier)
{
	if (IsIdle() && injuredSoldier && injuredSoldier->IsAlive())
	{
		patientTarget = injuredSoldier;
		finalTargetLocation = injuredSoldier->GetLocation();

		std::cout << "Medic (Team " << (team == TEAM_RED ? "RED" : "BLUE")
			<< ") assigned HEAL mission. Moving to base first.\n";

		// Start the FSM by transitioning to the "GoToBase" state
		SetState(new MedicGoToBaseState());
	}
	else
	{
		// Log if assignment failed
		std::cout << "Medic: Cannot be assigned mission. (Not idle or target is invalid)\n";
	}
}

// TODO: heal itself method
// TODO: implement actual methods
void Medic::ReportSighting(NpcType enemyType, Point enemyLoc)
{
	if (myCommander)
	{
		myCommander->ReportSighting(enemyType, enemyLoc);
	}
}
void Medic::ReportLowAmmo(NPC* warrior)
{
	return;
}
void Medic::ReportInjury(NPC* injuredSoldier)
{
	if (injuredSoldier->GetTeam() == this->GetTeam())
		return;
}













