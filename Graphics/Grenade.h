#pragma once
#include "Bullet.h"
#include "NPC.h"

const int NUM_BULLETS = 20;

class Grenade
{
private:
	double x, y;
	bool isExploding;
	Bullet* bullets[NUM_BULLETS];
	TeamColor team;
public:
	Grenade(double posX, double posY, TeamColor grenadeTeam);
	~Grenade();
	void Show();

	void Explode(const double* pMap, const std::vector<NPC*>& npcs);
	void SetIsExploding(bool value);

	void CreateSecurityMap(const double* pMap, double smap[MSX][MSY]);
	bool IsActive();
};