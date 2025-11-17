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
	for (Grenade* g : activeGrenades)
	{
		delete g;
	}
	activeGrenades.clear();
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
	double targetDist = RIFLE_RANGE * 0.7;

	if (grenades > 1)
	{
		targetDist = RIFLE_RANGE * 1.2;
	}
	
	attackPos.x = (int)(enemyLoc.x - (dx / euclideanDist) * targetDist);
	attackPos.y = (int)(enemyLoc.y - (dy / euclideanDist) * targetDist);

	// Clamp to SAFE boundaries
	if (attackPos.x < 1) attackPos.x = 1;
	if (attackPos.x >= MSX - 1) attackPos.x = MSX - 2;
	if (attackPos.y < 1) attackPos.y = 1;
	if (attackPos.y >= MSY - 1) attackPos.y = MSY - 2;

	double calculated_dist = Distance(location, attackPos);
	const double MAX_STRATEGIC_MOVE = 20.0; 

	if (calculated_dist > MAX_STRATEGIC_MOVE)
	{

		double dx_cap = attackPos.x - location.x;
		double dy_cap = attackPos.y - location.y;

		attackPos.x = (int)(location.x + (dx_cap / calculated_dist) * MAX_STRATEGIC_MOVE);
		attackPos.y = (int)(location.y + (dy_cap / calculated_dist) * MAX_STRATEGIC_MOVE);
	}

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

		Point startPos = this->GetLocation();
		Grenade* pGrenade = new Grenade(
			startPos.x, startPos.y, enemyLocation.x, enemyLocation.y, this->team);

		activeGrenades.push_back(pGrenade);
	}
	else
	{
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

void Warrior::DrawGrenades() const
{
	for (Grenade* pGrenade : activeGrenades)
	{
		pGrenade->Show();
	}
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

void Warrior::UpdateGrenades(const double* pMap, const std::vector<NPC*>& npcs)
{
	for (auto it = activeGrenades.begin(); it != activeGrenades.end(); )
	{
		Grenade* pGrenade = *it;
		pGrenade->Update(pMap, npcs);

		if (!pGrenade->IsActive())
		{
			delete pGrenade; 
			it = activeGrenades.erase(it);
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
	DrawGrenades();
}

void Warrior::DoSomeWork(const double* pMap)
{
	UpdateShots();
	if (npcList != nullptr)
	{
		UpdateGrenades(pMap, *npcList);
	}

	if (!IsAlive()) return;

	BuildViewMap(pMap);

	if (isSurviveMode && !dynamic_cast<WarriorRetreatingState*>(currentState))
	{
		HandleSurviveModeLogic();
	}

	if (currentState)
	{
		currentState->Execute(this);
	}

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

	if (commandCode == CMD_MOVE && !currentPath.empty())
	{
		Point finalTarget = currentPath.back();
		if (finalTarget.x == target.x && finalTarget.y == target.y)
		{
			std::cout << "Warrior already moving to target (" << target.x << ", " << target.y << "), ignoring redundant MOVE command.\n";
			return;
		}
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
		if (ammo == 0 && grenades == 0) break;

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
			else
			{
				std::cout << "Already at attack position, engaging.\n";
				SetState(new WarriorAttackingState());
			}
		}
		else
		{
			std::cout << "ERROR: No attack path found!\n";
		}
		break;
	}

	case CMD_DEFEND:
	case CMD_RETREAT:
	{
		std::cout << "Warrior RETREATING from (" << location.x << ", " << location.y << ") due to command: " << commandCode << "\n";

		double searchRange = 50.0; // טווח חיפוש הנקודה הבטוחה
		// 1. חיפוש נקודה בטוחה קרובה (ממומש ב-IPathfinding באמצעות BFS)
		Point safePos = FindClosestSafePosition(searchRange, &viewMap[0][0]);

		if (safePos.x != location.x || safePos.y != location.y)
		{
			std::cout << "Safe position found at (" << safePos.x << ", " << safePos.y << ")\n";

			// 2. מציאת מסלול בטוח ביותר לנקודה שנמצאה (A*)
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
				std::cout << "ERROR: No retreat path found, remaining IDLE!\n";
				SetState(new WarriorIdleState());
			}
		}
		else
		{
			std::cout << "Already at safe position, remaining IDLE\n";
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

void Warrior::HandleSurviveModeLogic()
{
	// 1. Priority: Self-preservation (retreat if low health/ammo)
	if (health < INJURED_THRESHOLD || ammo == 0) // (using health/ammo logic)
	{
		std::cout << "Warrior (" << (team == TEAM_RED ? "RED" : "BLUE")
			<< ") is low on resources - seeking safety/resupply.\n";
		ExecuteCommand(CMD_RETREAT, location); //
		return;
	}

	// 2. Priority: Engage any visible enemy
	NPC* pEnemy = ScanForEnemies(); //
	if (pEnemy != nullptr)
	{
		std::cout << "Warrior (" << (team == TEAM_RED ? "RED" : "BLUE")
			<< ") spotted enemy in Survive Mode - Initiating ATTACK.\n";

		ExecuteCommand(CMD_ATTACK, pEnemy->GetLocation()); //
		return;
	}

	// 3. Priority: If idle, hunt for enemies (move to a random strategic point)
	if (dynamic_cast<WarriorIdleState*>(currentState)) //
	{
		if (idlePatrolTimer > 0)
		{
			return;
		}
		// This uses the function you had commented out
		Point strategicTarget = GetRandomMapTarget();

		// Only move if a valid new target was found
		if (strategicTarget.x != location.x || strategicTarget.y != location.y)
		{
			std::cout << "Warrior (" << (team == TEAM_RED ? "RED" : "BLUE")
				<< ") no current enemy - ADVANCING to strategic point ("
				<< strategicTarget.x << ", " << strategicTarget.y << ").\n";
			ExecuteCommand(CMD_MOVE, strategicTarget); //
		}
	}
}

Point Warrior::GetRandomMapTarget()
{

	const double* safetyMap = GetViewMap();

	Point randomTarget;
	const int MAX_ATTEMPTS = 20;

	for (int i = 0; i < MAX_ATTEMPTS; ++i)
	{
		randomTarget.x = (double)(rand() % MSX);
		randomTarget.y = (double)(rand() % MSY);

		if (IsWalkable(safetyMap, randomTarget) && (randomTarget.x != location.x || randomTarget.y != location.y))
		{
			std::cout << "Warrior found a RANDOM WALKABLE point for patrol: ("
				<< randomTarget.x << ", " << randomTarget.y << ") using IsWalkable.\n";
			return randomTarget;
		}
	}

	return location;
}
