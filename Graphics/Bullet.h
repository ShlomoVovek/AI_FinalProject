#pragma once
#include "Definition.h"
#include "NPC.h"

const double BULLET_SPEED = 0.35;
const double BULLET_LIFETIME = 0.5;
const double BULLET_SIZE = 0.2;
const double BULLET_DAMAGE = 3;

class Bullet
{
private:
	double x, y;
	bool isMoving;
	bool isCreatingSecurityMap;
	double dirX, dirY;
	double timeAlive; // Track how long bullet has been alive
	TeamColor team;

public:
	Bullet(double xPos, double yPos, double alpha, TeamColor bulletTeam);

	void Move(const double* pMap, const std::vector<NPC*>& npcs);
	void Show();
	void SetIsMoving(bool value) { isMoving = value; if (value) timeAlive = 0; }

	void CreateSecurityMap(const double* pMap, double smap[MSX][MSY]);
	bool IsActive() { return isMoving; } // Check if bullet should be displayed
	double GetTimeAlive() { return timeAlive; } // Get current time alive
};