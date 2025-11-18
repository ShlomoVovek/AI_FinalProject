#pragma once
#include "Bullet.h"
#include "NPC.h"

const int NUM_BULLETS = 20;
const double GRENADE_SPEED = 2;
const double GRENADE_DIRECT_HIT_DAMAGE = 20.0;

class Grenade
{
private:
	double currentX, currentY;
	double targetX, targetY;
	double dirX, dirY;

	bool isExploding;
	bool justExploded;
	Bullet* bullets[NUM_BULLETS];
	TeamColor team;
public:
	Grenade(double startX, double startY, double endX, double endY, TeamColor grenadeTeam);
	~Grenade();
	void Show();

	void Update(const double* pMap, const std::vector<NPC*>& npcs);
	void Explode(const double* pMap, const std::vector<NPC*>& npcs);
	void SetIsExploding(bool value);

	bool DidJustExplode() { return justExploded; }
	void ClearJustExplodedFlag() { justExploded = false; }

	void CreateSecurityMap(const double* pMap, double smap[MSX][MSY]);
	bool IsActive();
};