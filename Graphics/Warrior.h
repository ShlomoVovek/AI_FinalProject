#pragma once
#include "NPC.h"
#include "IPathfinding.h"

// ammo
const int MAX_AMMO = 30;
const int CRITICAL_AMMO = (int) MAX_AMMO * 0.25;
const int MAX_GRENADE = 3;
const int CRITICAL_GRENADE = 1;

// range
const double RILFE_RANGE = 40.0;
const double GRENADE_RANGE = 20.0;
const double ATTACK_THRESHOLD = 5.0;

// possilbe states
enum WorriorState
{
	W_IDLE,
	W_ADVANCE, // CMD_MOVE
	W_ATTACK,  // CMD_ATTACK
	W_RETREAT, // CMD_DEFEND
	W_INJURED,
	W_RESUPPLY
};


class Warrior : public NPC, public IPathfinding
{
private:
	int ammo;
	int grenades;
	bool isAttacking;
	Point currentAttackTarget;
	WorriorState currentState;

	int moveCooldown; // TODO: add in NPC.h

	Point DetermineBestAttackPosition(Point enemyLoc);

	// bool FindAStarPath(Point goal, const double* safteyMap) override;
	// Point FindClosestSafePosition(double searchRange, const double* safetyMap) override;

protected:
	void CalculatePathAndMove() override;
	void Shoot(Point enemyLoc);
	void ThrowGrenade(Point enemyLoc);

	// connect interface to NPC's methods
	// bool IsWalkable(Point p) const { return IPathfinding::IsWalkable(this->GetViewMap(), p); }
	Point GetLocation() const override { return NPC::GetLocation(); }

public:
	// constructor
	Warrior(int x, int y, TeamColor t);

	void DoSomeWork(const double* pMap) override;
	void RestorePath(Cell* goalCell, std::list<Point>& path);

	// report to Commander
	void ReportSighting(NpcType enemyType, Point enemyLoc) override;
	void ReportInjury(NPC* injuredSoldier) override;
	void ReportLowAmmo(NPC* warrior) override;
	
	// execute commmands
	void ExecuteCommand(int commandCode, Point target) override;
	void Resupply(int amount);
};
