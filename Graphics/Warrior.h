#pragma once
#include "NPC.h"
#include "IPathfinding.h"

class WarriorState;

// ammo
const int MAX_AMMO = 15;
const int CRITICAL_AMMO = (int) MAX_AMMO * 0.25;
const int MAX_GRENADE = 3;
const int CRITICAL_GRENADE = 1;
const double GUN_HIT_DAMAGE = 7.5;
const double GRENADE_HIT_DAMAGE = 30.0;

// range
const double RIFLE_RANGE = 40.0;
const double GRENADE_RANGE = 15.0;
const double ATTACK_THRESHOLD = 5.0;

// Shot visualization
const int SHOT_DISPLAY_FRAMES = 15;

struct ShotInfo
{
	Point start;
	Point target;
	int framesRemaining;

	ShotInfo(Point s, Point t) : start(s), target(t), framesRemaining(SHOT_DISPLAY_FRAMES) {}
};

class Warrior : public NPC, public IPathfinding
{
private:
	int ammo;
	int grenades;
	bool isAttacking;
	Point currentAttackTarget;
	
	// fsm management
	WarriorState* currentState;
	bool requestedMedic;
	bool requestedSupply;

	// Shot visualization
	std::vector<ShotInfo> activeShots;

	Point DetermineBestAttackPosition(Point enemyLoc);

protected:
	void CalculatePathAndMove() override;
	Point GetLocation() const override { return NPC::GetLocation(); }

public:
	friend class WarriorIdleState;
	friend class WarriorAdvancingState;
	friend class WarriorAttackingState;
	friend class WarriorRetreatingState;

	// constructor
	Warrior(int x, int y, TeamColor t);
	~Warrior();

	void DoSomeWork(const double* pMap) override;
	void RestorePath(Cell* goalCell, std::list<Point>& path);

	// report to Commander
	void ReportSighting(NpcType enemyType, Point enemyLoc) override;
	void ReportInjury(NPC* injuredSoldier) override;
	void ReportLowAmmo(NPC* warrior) override;
	
	// execute commmands
	void ExecuteCommand(int commandCode, Point target) override;

	// combat methods
	void Shoot(NPC* pEnemy);
	void ThrowGrenade(Point enemyLoc);

	// visualization
	void DrawShots() const;
	void UpdateShots();

	void Show() const override;

	// fsm management
	void SetState(WarriorState* newState);
	WarriorState* GetState() const { return currentState; }

	// helper methods
	NPC* ScanForEnemies() const;
	bool CanShootAt(Point target) const;

	void RequestMedic();
	void RequestSupply();
	bool HasRequestedMedic() const { return requestedMedic; }
	bool HasRequestedSupply() const { return requestedSupply; }

	void Resupply(int amount);

	// getters
	int GetAmmo() const { return ammo; }
	int GetGrenades() const { return grenades; }
	double GetHealth() const { return health; }
	bool IsMoving() const { return isMoving; }
	const std::list<Point>& GetCurrentPath() const { return currentPath; }
};
