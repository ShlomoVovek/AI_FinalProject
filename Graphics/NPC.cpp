#include "NPC.h"
#include "Definition.h"
#include "Commander.h"
// #include "Bullet.h"
#include <math.h>
#include <iostream>


NPC::NPC(int x, int y, TeamColor t, NpcType nt) :
	team(t), type(nt), health(MAX_HP), isMoving(false), myCommander(nullptr), npcList(nullptr),
	directionX(0.0), directionY(0.0), gameMapPtr(nullptr), isSurviveMode(false)
{
	location.x = x;
	location.y = y;
	targetLocation = location;

	// view map
	for (int i = 0; i < MSX; ++i)
		for (int j = 0; j < MSY; ++j)
			viewMap[i][j] = 0.0;
}

void NPC::SetCommander(Commander* pCommander)
{
	myCommander = pCommander;
}

// TODO: check what it does and why important
void NPC::SetDirection(Point target)
{
	// map boundaries
	if (target.x < 0) target.x = 0;
	if (target.x >= MSX) target.x = MSX - 1;
	if (target.y < 0) target.y = 0;
	if (target.y >= MSY) target.y = MSY - 1;

	targetLocation = target;
	double dx = targetLocation.x - location.x;
	double dy = targetLocation.y - location.y;

	double manhattanDist = ManhattanDistance(
		(int)location.x, (int)location.y,
		targetLocation.x, targetLocation.y);
	if (manhattanDist > 0.1)
	{
		// For diagonal movement, normalize the vector
		// TODO: DELETE diagonal movement
		double euclideanLength = sqrt(dx * dx + dy * dy);
		if (euclideanLength > 0.1)
		{
			directionX = dx / euclideanLength;
			directionY = dy / euclideanLength;
			isMoving = true;
		}
		else
		{
			isMoving = false;
		}
	}
	else
	{
		isMoving = false;
	}
}

// TODO: fix movement: obligate movement near frame
void NPC::MoveToTarget()
{
	if (isMoving && gameMapPtr != nullptr)
	{

		double nextX = location.x + (int)(SPEED * directionX);
		double nextY = location.y + (int)(SPEED * directionY);

		int nextGridX = (int)nextX;
		int nextGridY = (int)nextY;
		int currentGridX = (int)location.x;
		int currentGridY = (int)location.y;


		double manhattanDist = ManhattanDistance(currentGridX, currentGridY, targetLocation.x, targetLocation.y);
		if (manhattanDist < SPEED)
		{
			isMoving = false;
			location.x = targetLocation.x;
			location.y = targetLocation.y;

			gameMapPtr->RegisterAgentMove(currentGridX, currentGridY, targetLocation.x, targetLocation.y);
			return;
		}

		if (gameMapPtr->IsWalkable(nextGridX, nextGridY))
		{
			gameMapPtr->RegisterAgentMove(currentGridX, currentGridY, nextGridX, nextGridY);

			location.x = nextX;
			location.y = nextY;
		}
		else
		{
			isMoving = false;
		}
	}
}

void RestorePath(Cell* goalCell, std::list<Point>& path)
{

}

void NPC::BuildViewMap(const double* pMap)
{
	// 1. reset NPC's view map
	// TODO: check if really need to erase all every time or just once in a game or in couple frames.
	// it is good that NPC will have memory about obstacles
	/*
	for (int i = 0; i < MSX; i++)
		for (int j = 0; j < MSY; j++)
			viewMap[i][j] = COST_UNKNOWN;
	*/
	for (int i = 0; i < MSX; i++)
	{
		for (int j = 0; j < MSY; j++)
		{
			viewMap[i][j] = pMap[i * MSY + j];
		}
	}


	// 2. create rays for view
	const int NUM_RAYS = 36;
	double teta = 2 * PI / NUM_RAYS;

	for (int i = 0; i < NUM_RAYS; i++)
	{
		double alpha = i * teta;
		double tmpX = location.x, tmpY = location.y;
		double dirX = cos(alpha);
		double dirY = sin(alpha);

		for (int step = 0; step < SIGHT_RANGE; ++step) // limited sight range
		{
			//tmpX += SPEED * dirX * 10;
			//tmpY += SPEED * dirY * 10;

			tmpX += dirX;
			tmpY += dirY;

			if (tmpX >= 0 && tmpX < MSX && tmpY >= 0 && tmpY < MSY)
			{
				int mapX = (int)tmpX;
				int mapY = (int)tmpY;

				int obstacleIndex = mapX * MSY + mapY;
				CellType obstacleType = (CellType)pMap[obstacleIndex];

				switch (obstacleType)
				{
				case ROCK:
					viewMap[mapX][mapY] = ROCK;
					break; // stops the rays

				case WATER:
					viewMap[mapX][mapY] = WATER;
					continue; // rays continue

				case TREE:
					viewMap[mapX][mapY] = COST_DANGER;
					break; // stops the rays

				case EMPTY:
					viewMap[mapX][mapY] = COST_SAFE;
					continue; // rays continue

				case BLUE_BASE:
				case RED_BASE:
					viewMap[mapX][mapY] = COST_SAFE;
					continue; // rays continue
				}
				break; // is ROCK or TREES
			}
			else // out of borders
				break;
		}
	}
}

void NPC::TakeDamage(double dmg)
{
	health -= dmg;

	if (health <= 0 && health + dmg > 0)
	{
		if (gameMapPtr != nullptr)
		{
			gameMapPtr->RegisterAgentDeath((int)location.x, (int)location.y);
		}
	}
}

void NPC::DrawNpcBase() const
{
	if (!IsAlive()) return;

	if (team == TEAM_RED)
		glColor3d(1.0, 0.4, 0.0);
	else // TEAM_BLUE
		glColor3d(0.0, 0.5, 1.0);

	// draw squre
	glBegin(GL_POLYGON);
	glVertex2d(location.x + 0.5, location.y - 0.5);
	glVertex2d(location.x + 0.5, location.y + 0.5);
	glVertex2d(location.x - 0.5, location.y + 0.5);
	glVertex2d(location.x - 0.5, location.y - 0.5);
	glEnd();

	// draw black frame
	glColor3d(0.0, 0.0, 0.0);
	glBegin(GL_LINE_LOOP);
	glVertex2d(location.x + 0.5, location.y - 0.5);
	glVertex2d(location.x + 0.5, location.y + 0.5);
	glVertex2d(location.x - 0.5, location.y + 0.5);
	glVertex2d(location.x - 0.5, location.y - 0.5);
	glEnd();
}

void NPC::DrawLetter() const
{
	if (!IsAlive()) return;

	double xOffset = -0.25;
	double yOffset = -0.3;

	glColor3d(0.0, 0.0, 0.0); // color
	glRasterPos2d(location.x + xOffset, location.y + yOffset);
	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, type); // font
}

void NPC::DrawHealthBar() const
{
	if (!IsAlive()) return;

	glLineWidth(5.0);
	if (CanFight())
		glColor3d(0.0, 1.0, 0.0); // green
	else
		glColor3d(1.0, 0.0, 0.0); // red

	double fullLength = 1.0;
	double currentHealthLength = (health / MAX_HP) * fullLength;

	// little above the Cell
	double yPosition = location.y + 0.75;

	glBegin(GL_LINES);
	glVertex2d(location.x - 0.5, yPosition);
	glVertex2d(location.x - 0.5 + currentHealthLength, yPosition);
	glEnd();

	glLineWidth(1.0);

	// frame
	glColor3d(0.0, 0.0, 0.0);
	glBegin(GL_LINE_STRIP);
	glVertex2d(location.x - 0.5, yPosition + 0.1);
	glVertex2d(location.x + 0.5, yPosition + 0.1);
	glEnd();
	glLineWidth(1.0);
}
void NPC::Show() const
{
	DrawNpcBase();
	DrawLetter();
	DrawHealthBar();
}
