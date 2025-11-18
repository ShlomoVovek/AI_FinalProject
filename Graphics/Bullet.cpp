#include "Bullet.h"
#include <math.h>
#include "glut.h"
#include <vector>
#include <iostream>

Bullet::Bullet(double xPos, double yPos, double alpha, TeamColor bulletTeam) :
	x(xPos), y(yPos), dirX(cos(alpha)), dirY(sin(alpha)),
	isMoving(false), isCreatingSecurityMap(false),
	timeAlive(0), team(bulletTeam)
{
	x = xPos;
	y = yPos;

	dirX = cos(alpha);
	dirY = sin(alpha);

	isMoving = false;
	isCreatingSecurityMap = false;
	timeAlive = 0;
}

void Bullet::Move(const double* pMap, const std::vector<NPC*>& npcs)
{
	if (!isMoving) return;

	timeAlive += 0.01;
	if (timeAlive > BULLET_LIFETIME)
	{
		isMoving = false;
		return;
	}

	double tmpX = x + BULLET_SPEED * dirX;
	double tmpY = y + BULLET_SPEED * dirY;

	const double NPC_HITBOX_RADIUS = 0.5;

	for (NPC* npc : npcs)
	{
		if (!npc->IsAlive() || npc->GetTeam() == this->team)
		{
			continue;
		}

		Point npcLoc = npc->GetLocation();
		double dist = Distance(tmpX, tmpY, npcLoc.x, npcLoc.y);

		if (dist < NPC_HITBOX_RADIUS)
		{

			std::cout << "Bullet HIT enemy!\n";
			npc->TakeDamage(BULLET_DAMAGE); 
			isMoving = false;
			return; 
		}
	}

	if (tmpX > 0 && tmpX < MSX && tmpY > 0 && tmpY < MSY)
	{
		double cellValue = pMap[(int)tmpX * MSY + (int)tmpY];

		if (cellValue == (double)ROCK || cellValue == (double)TREE)
		{
			isMoving = false;
		}
		else
		{
			x = tmpX;
			y = tmpY;
		}
	}
	else
	{
		isMoving = false;
	}
}

void Bullet::Show()
{
	if(team == TEAM_RED)
		glColor3d(1, 0, 0); 
	else
		glColor3d(0, 0, 1);

	glBegin(GL_POLYGON);
	glVertex2d(x - BULLET_SIZE, y);
	glVertex2d(x, y + BULLET_SIZE);
	glVertex2d(x + BULLET_SIZE, y);
	glVertex2d(x, y - BULLET_SIZE);
	glEnd();
}

void Bullet::CreateSecurityMap(const double* pMap, double smap[MSX][MSY])
{
	double tmpX = x, tmpY = y;
	double xsm = x, ysm = y;

	isCreatingSecurityMap = true;
	while (isCreatingSecurityMap)
	{

		tmpX += BULLET_SPEED * dirX;
		tmpY += BULLET_SPEED * dirY;


		if (tmpX > 0 && tmpX < MSX && tmpY > 0 && tmpY < MSY)
		{
			double cellValue = pMap[(int)tmpX * MSY + (int)tmpY];

			// Stop ONLY on solid objects
			if (cellValue == (double)ROCK || cellValue == (double)TREE)
			{
				isCreatingSecurityMap = false;
			}
			else // Keep going through EMPTY, WATER, BASE
			{
				xsm = tmpX;
				ysm = tmpY;
				smap[(int)xsm][(int)ysm] += 0.001;
			}
		}
		else // Hit map boundary
		{
			isCreatingSecurityMap = false;
		}
	}
}