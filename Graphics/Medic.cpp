#include "Medic.h"
#include "Definition.h"
#include "Map.h"
#include <iostream>
#include <queue>
#include <map>
#include <algorithm>

extern const int MSX;
extern const int MSY;

Medic::Medic(int x, int y, TeamColor t) : NPC(x, y, t, MEDIC)
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

	DecayThreats();
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

		MoveToTarget();

		if (Distance(location.x, location.y, nextPoint.x, nextPoint.y) < SPEED)
		{
			currentPath.pop_front();

			if (currentPath.empty())
			{
				this->isMoving = false;
			}
			else
			{
				SetDirection(currentPath.front());
			}
		}

	}
	else
	{
		this->isMoving = false;
	}
}

Point Medic::GetBaseLocation() const // TODO: return both bases
{									 
	int BASE = this->GetTeam() == TEAM_RED ? RED_BASE : BLUE_BASE;
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

NPC* Medic::GetNextPatient() const
{
	if (patientsQueue.empty()) return nullptr;
	return patientsQueue.front();
}

void Medic::RemoveCurrentPatient()
{
	if (!patientsQueue.empty())
	{
		patientsQueue.pop_front();
	}
}

void Medic::ExecuteCommand(int CommandCode, Point target)
{
	if (!IsAlive()) return;

	isMoving = false;

	switch (CommandCode)
	{
	case CMD_HEAL:
	{
		NPC* injuredSoldier = nullptr;
		if (npcList != nullptr)
		{
			for (NPC* npc : *npcList)
			{
				if (npc->IsAlive() &&
					npc->GetTeam() == this->GetTeam() &&
					npc->GetLocation().x == target.x &&
					npc->GetLocation().y == target.y)
				{
					injuredSoldier = npc;
					break;
				}
			}
		}

		if (injuredSoldier != nullptr)
		{

			if (std::find(patientsQueue.begin(), patientsQueue.end(), injuredSoldier) == patientsQueue.end())
			{
				patientsQueue.push_back(injuredSoldier);
			}

			if (IsIdle())
			{
				std::cout << "Medic (Team " << (team == TEAM_RED ? "RED" : "BLUE")
					<< ") received HEAL command. Moving to base first.\n";
				SetState(new MedicGoToBaseState());
			}
		}
		else
		{
			std::cout << "Medic: Could not find injured soldier at target location!\n";
		}
		break;
	}
	default:
		break;
	}
}

bool Medic::IsIdle() const
{
	return dynamic_cast<MedicIdleState*>(currentState) != nullptr;
}

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

void Medic::AssignHealMission(NPC* injuredSoldier)
{
	if (!injuredSoldier)
	{
		std::cout << "Medic: Cannot assign mission - invalid soldier pointer.\n";
		return;
	}

	if (!injuredSoldier->IsAlive())
	{
		std::cout << "Medic: Cannot assign mission - soldier is dead.\n";
		return;
	}

	if (injuredSoldier->GetHealth() >= MAX_HP)
	{
		std::cout << "Medic: Cannot assign mission - soldier is fully healed.\n";
		return;
	}

	if (std::find(patientsQueue.begin(), patientsQueue.end(), injuredSoldier) != patientsQueue.end())
	{
		std::cout << "Medic: Already assigned to this patient.\n";
		return;
	}

	patientsQueue.push_back(injuredSoldier);

	std::cout << "Medic (Team " << (team == TEAM_RED ? "RED" : "BLUE")
		<< ") assigned HEAL mission. Moving to base first.\n";

	SetState(new MedicGoToBaseState());
}

bool Medic::NeedsSelfHeal() const
{
	return GetHealth() < SELF_HEAL_THRESHOLD;
}


NPC* Medic::FindNearbyInjuredAlly(bool excludeCurrentTarget) const
{
	const double SURVIVAL_HEAL_RADIUS = 5.0;

	if (npcList == nullptr) return nullptr;

	Point myLoc = GetLocation();
	NPC* currentTarget = excludeCurrentTarget ? GetNextPatient() : nullptr;

	for (NPC* ally : *npcList)
	{
		if (ally != this &&
			ally->GetTeam() == this->GetTeam() &&
			ally->IsAlive() &&
			ally->GetHealth() < MAX_HP &&
			Distance(myLoc.x, myLoc.y, ally->GetLocation().x, ally->GetLocation().y) <= SURVIVAL_HEAL_RADIUS)
		{
			if (excludeCurrentTarget && ally == currentTarget)
			{
				continue;
			}
			return ally;
		}
	}

	return nullptr;
}








