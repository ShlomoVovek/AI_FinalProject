#include "Warrior.h"
#include "Definition.h"
#include "Commander.h"
#include "WarriorIdleState.h"
#include "WarriorAdvancingState.h"
#include "WarriorAttackingState.h"
#include "WarriorRetreatingState.h"
#include <iostream>
#include <queue>
#include <map>
#include <algorithm>

Warrior::Warrior(int x, int y, TeamColor t) :
	NPC(x, y, t, WARRIOR), ammo(MAX_AMMO), grenades(MAX_GRENADE),
	isAttacking(false), requestedMedic(false), requestedSupply(false)
{
	currentState = new WarriorIdleState();
	currentState->OnEnter(this);
}

Warrior::~Warrior()
{
	if (currentState)
	{
		delete currentState;
		currentState = nullptr;
	}
}

void Warrior::SetState(WarriorState* newState)
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

// TODO: build this method
Point Warrior::DetermineBestAttackPosition(Point enemyLoc)
{
	// Calculate a tactical position near the enemy
	double dx = enemyLoc.x - location.x;
	double dy = enemyLoc.y - location.y;
	
	double manhattanDist = ManhattanDistance(
		(int)location.x, (int)location.y,
		enemyLoc.x, enemyLoc.y
	);

	if (manhattanDist < 1) manhattanDist = 1;
	
	double euclideanDist = EuclideanDist(dx,dy);
	if (euclideanDist < 1.0) euclideanDist = 1.0;

	Point attackPos;
	double targetDist = RIFLE_RANGE * 0.7; // Stay at 70% of max range
	attackPos.x = (int)(enemyLoc.x - (dx / euclideanDist) * targetDist);
	attackPos.y = (int)(enemyLoc.y - (dy / euclideanDist) * targetDist);

	// Clamp to SAFE boundaries
	if (attackPos.x < 1) attackPos.x = 1;
	if (attackPos.x >= MSX - 1) attackPos.x = MSX - 2;
	if (attackPos.y < 1) attackPos.y = 1;
	if (attackPos.y >= MSY - 1) attackPos.y = MSY - 2;

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

// TODO: delete shoots when is not alive
void Warrior::Shoot(NPC* pEnemy)
{
	if (pEnemy == nullptr) return;
	if (ammo > 0)
	{
		activeShots.push_back(ShotInfo(location, pEnemy->GetLocation()));

		ammo--;
		std::cout << "Warrior (Team " << (team == TEAM_RED ? "RED" : "BLUE")
			<< ") shooting! Ammo left: " << ammo << "\n";

		pEnemy->TakeDamage(GUN_HIT_DAMAGE);

		if (!pEnemy->IsAlive())
		{
			std::cout << "Enemy neutralized!\n";
		}

		// Check if low on ammo
		if (ammo <= CRITICAL_AMMO && !requestedSupply)
		{
			RequestSupply();
		}
	}
	else
	{
		std::cout << "Warrior out of ammo!\n";
		RequestSupply();
	}
}
void Warrior::ThrowGrenade(Point enemyLocation)
{
	if (grenades > 0)
	{
		grenades--;
		std::cout << "Warrior (Team " << (team == TEAM_RED ? "RED" : "BLUE")
			<< ") throwing grenade! Grenades left: " << grenades << "\n";
	}
	else
	{
		std::cout << "Warrior out of grenades!\n";
		RequestSupply();
	}
}
// TODO: each shot increase danger in map where it crosses, color in red
void Warrior::DrawShots() const
{
	if (!this->IsAlive()) return;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Draw all active shots
	for (const auto& shot : activeShots)
	{
		// Calculate alpha based on remaining frames for fade effect
		double alpha = (double)shot.framesRemaining / SHOT_DISPLAY_FRAMES;

		// Draw red line from warrior to target
		if (this->GetTeam() == TEAM_RED)
			glColor4d(1.0, 0.0, 0.0, alpha); // Red with alpha
		else
			glColor4d(0.0, 0.0, 1.0, alpha);

		glLineWidth(2.0);

		glBegin(GL_LINES);
		glVertex2d(shot.start.x, shot.start.y);
		glVertex2d(shot.target.x, shot.target.y);
		glEnd();
	}

	glDisable(GL_BLEND);
	glLineWidth(1.0); // Reset line width
}

void Warrior::UpdateShots()
{
	// Update all active shots and remove expired ones
	for (auto it = activeShots.begin(); it != activeShots.end(); )
	{
		it->framesRemaining--;

		if (it->framesRemaining <= 0)
		{
			it = activeShots.erase(it);
		}
		else
		{
			++it;
		}
	}
}
void Warrior::Show() const
{
	NPC::Show();
	
	DrawShots(); // Draw shooting lines on top
}

void Warrior::DoSomeWork(const double* pMap)
{
	if (!IsAlive()) return;

	BuildViewMap(pMap);

	UpdateShots();

	if (!CanFight() && myCommander != nullptr && !hasReportedInjury)
	{
		myCommander->ReportInjury(this);
		hasReportedInjury = true;

		std::cout << "Warrior (Team " << (team == TEAM_RED ? "RED" : "BLUE")
			<< ") reported injury! Health: " << health << "/" << MAX_HP << "\n";
	}

	if (CanFight() && hasReportedInjury)
	{
		hasReportedInjury = false;
	}

	if (currentState)
	{
		currentState->Execute(this);
	}
}
void Warrior::ExecuteCommand(int commandCode, Point target)
{
	if (!IsAlive()) return;

	double distToTarget = Distance(location, target);
	if (distToTarget < 2.0 && dynamic_cast<WarriorIdleState*>(currentState))
	{
		std::cout << "Warrior already at target, ignoring redundant command\n";
		return;
	}

	// Clamp target to map boundaries
	if (target.x < 1) target.x = 1;
	if (target.x >= MSX - 1) target.x = MSX - 2;
	if (target.y < 1) target.y = 1;
	if (target.y >= MSY - 1) target.y = MSY - 2;

	std::cout << "Warrior (Team " << (team == TEAM_RED ? "RED" : "BLUE")
		<< ") at (" << location.x << ", " << location.y
		<< ") received command: " << commandCode
		<< " target: (" << target.x << ", " << target.y << ")\n";

	// Clear old path
	currentPath.clear();

	switch (commandCode)
	{
	case CMD_MOVE:
		std::cout << "Warrior ADVANCING to (" << target.x << ", " << target.y << ")\n";

		if (FindAStarPath(target, &viewMap[0][0]))
		{
			if (!currentPath.empty())
			{
				SetDirection(currentPath.front());
				SetState(new WarriorAdvancingState());
				std::cout << "Path found! Steps: " << currentPath.size() << "\n";
			}
			else
			{
				std::cout << "Warrior already at target, remaining IDLE\n";
				if (!dynamic_cast<WarriorIdleState*>(currentState))
				{
					SetState(new WarriorIdleState());
				}
			}
		}
		else
		{
			std::cout << "ERROR: No path found to target!\n";
		}
		break;

	case CMD_ATTACK:
	{
		if (ammo == 0) break;

		currentAttackTarget = target;
		Point attackPos = DetermineBestAttackPosition(target);

		std::cout << "Warrior ATTACKING - moving to position ("
			<< attackPos.x << ", " << attackPos.y << ")\n";

		if (FindAStarPath(attackPos, &viewMap[0][0]))
		{
			if (!currentPath.empty())
			{
				SetDirection(currentPath.front());
				SetState(new WarriorAttackingState());
				std::cout << "Attack path found! Steps: " << currentPath.size() << "\n";
			}
		}
		else
		{
			std::cout << "ERROR: No attack path found!\n";
		}
		break;
	}

	case CMD_DEFEND:
	{
		std::cout << "Warrior RETREATING from (" << location.x << ", " << location.y << ")\n";

		double searchRange = 50.0;
		Point safePos = FindClosestSafePosition(searchRange, &viewMap[0][0]);

		if (safePos.x != location.x || safePos.y != location.y)
		{
			std::cout << "Safe position found at (" << safePos.x << ", " << safePos.y << ")\n";

			if (FindAStarPath(safePos, &viewMap[0][0]))
			{
				if (!currentPath.empty())
				{
					SetDirection(currentPath.front());
					SetState(new WarriorRetreatingState());
					std::cout << "Retreat path found! Steps: " << currentPath.size() << "\n";
				}
			}
			else
			{
				std::cout << "ERROR: No retreat path found!\n";
			}
		}
		else
		{
			std::cout << "Already at safe position\n";
			SetState(new WarriorIdleState());
		}
		break;
	}

	case CMD_HEAL:
		// Warrior received heal - reset medic request
		std::cout << "Warrior received healing\n";
		requestedMedic = false;
		health = MAX_HP;
		break;

	case CMD_RESUPPLY:
		// Warrior received ammo - reset supply request
		std::cout << "Warrior received ammo resupply\n";
		requestedSupply = false;
		Resupply(MAX_AMMO - ammo, MAX_GRENADE - grenades);
		break;

	default:
		std::cout << "Unknown command: " << commandCode << "\n";
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
		myCommander->ReportInjury(injuredSoldier);
	}
}

void Warrior::RequestMedic()
{
	if (!requestedMedic)
	{
		std::cout << "Warrior (Team " << (team == TEAM_RED ? "RED" : "BLUE")
			<< ") requesting medic! Health: " << health << "\n";
		requestedMedic = true;
		ReportInjury(this);
	}
}

void Warrior::RequestSupply()
{
	if (!requestedSupply)
	{
		std::cout << "Warrior (Team " << (team == TEAM_RED ? "RED" : "BLUE")
			<< ") requesting ammo! Ammo: " << ammo << "\n";
		requestedSupply = true;
		ReportLowAmmo(this);
	}
}

void Warrior::Resupply(int ammoAmount, int grenadeAmount)
{
	ammo = std::min(ammo + ammoAmount, MAX_AMMO);
	grenades = std::min(grenades + grenadeAmount, MAX_GRENADE);

	std::cout << "Warrior resupplied. Ammo: " << ammo
		<< ", Grenades: " << grenades << "\n";

	if (ammo > CRITICAL_AMMO)
		requestedSupply = false;
}

NPC* Warrior::ScanForEnemies() const
{
	if (!npcList) { return nullptr; }

	for (NPC* agent : *npcList)
	{
		if (!agent->IsAlive() || agent->GetTeam() == this->GetTeam())
		{
			continue;
		}

		Point enemyPos = agent->GetLocation();
		int ex = (int)enemyPos.x;
		int ey = (int)enemyPos.y;

		if (ex >= 0 && ex < MSX && ey >= 0 && ey < MSY &&
			viewMap[ex][ey] > 0.5)
		{
			// found enemy
			return agent;           
		}
	}
	return nullptr;
}

bool Warrior::CanShootAt(Point target) const
{
	// Check distance
	double dist = Distance(location, target);
	if (dist > RIFLE_RANGE)
		return false;

	// Check line of sight - use view map
	int tx = (int)target.x;
	int ty = (int)target.y;

	if (tx >= 0 && tx < MSX && ty >= 0 && ty < MSY)
	{
		return viewMap[tx][ty] > 0.5; // Can see target
	}

	return false;
}