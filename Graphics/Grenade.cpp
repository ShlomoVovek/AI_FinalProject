#include "Grenade.h"
#include "NPC.h"

Grenade::Grenade(double startX, double startY, double endX, double endY, TeamColor grenadeTeam) :
	currentX(startX), currentY(startY),
	targetX(endX), targetY(endY),
	isExploding(false), team(grenadeTeam), justExploded(false)
{
	double dx = targetX - currentX;
	double dy = targetY - currentY;
	double dist = sqrt(dx * dx + dy * dy);

	if (dist < 0.01)
	{
		dirX = 0;
		dirY = 0;
		isExploding = true;
	}
	else
	{
		dirX = dx / dist;
		dirY = dy / dist;
	}
	double alpha = 0, teta = 2 * PI / NUM_BULLETS;
	for (int i = 0; i < NUM_BULLETS; i++, alpha += teta)
	{
		bullets[i] = new Bullet(targetX, targetY, alpha, team);
	}
}

Grenade::~Grenade()
{
	int i;
	for (i = 0; i < NUM_BULLETS; i++)
	{
		delete bullets[i];
	}
}

void Grenade::Show()
{
	if (isExploding)
	{
		for (int i = 0; i < NUM_BULLETS; i++)
			bullets[i]->Show();
	}
	else
	{
		if (team == TEAM_RED)
			glColor3d(1, 0, 0);
		else
			glColor3d(0, 0, 1);

		const double GRENADE_VISUAL_SIZE = 0.3;

		glBegin(GL_POLYGON);
		glVertex2d(currentX - GRENADE_VISUAL_SIZE, currentY);
		glVertex2d(currentX, currentY + GRENADE_VISUAL_SIZE);
		glVertex2d(currentX + GRENADE_VISUAL_SIZE, currentY);
		glVertex2d(currentX, currentY - GRENADE_VISUAL_SIZE);
		glEnd();
	}
}

void Grenade::Update(const double* pMap, const std::vector<NPC*>& npcs)
{
	if (!isExploding)
	{
		currentX += dirX * GRENADE_SPEED;
		currentY += dirY * GRENADE_SPEED;

		double new_dx = targetX - currentX;
		double new_dy = targetY - currentY;

		// 1. Check if reached target
		if ((new_dx * dirX + new_dy * dirY) < 0)
		{
			currentX = targetX;
			currentY = targetY;
			SetIsExploding(true);
		}
		const double NPC_HITBOX_RADIUS = 0.5;
		for (NPC* npc : npcs)
		{
			if (!npc->IsAlive() || npc->GetTeam() == this->team)
			{
				continue;
			}

			Point npcLoc = npc->GetLocation();
			double dist = Distance(currentX, currentY, npcLoc.x, npcLoc.y);

			if (dist < NPC_HITBOX_RADIUS)
			{
				npc->TakeDamage(GRENADE_DIRECT_HIT_DAMAGE);
				SetIsExploding(true);
				break;
			}
		}
		if (!isExploding)
		{
			if (currentX <= 0 || currentX >= MSX || currentY <= 0 || currentY >= MSY)
			{
				SetIsExploding(true);
			}
			else
			{
				double cellValue = pMap[(int)currentX * MSY + (int)currentY];
				if (cellValue == (double)ROCK || cellValue == (double)TREE)
				{
					SetIsExploding(true);
				}
			}
		}
	}

	if (isExploding)
	{
		if (DidJustExplode())
		{
			Point explosionLocation = { (int)targetX, (int)targetY };
			for (NPC* npc : npcs)
			{
				if (npc->IsAlive())
				{
					npc->ReportExplosion(explosionLocation);
				}
			}
			ClearJustExplodedFlag();
		}

		Explode(pMap, npcs);
	}
}


void Grenade::Explode(const double* pMap, const std::vector<NPC*>& npcs)
{
	int i;
	for (i = 0; i < NUM_BULLETS; i++)
		bullets[i]->Move(pMap, npcs); 

}

bool Grenade::IsActive()
{
	if (!isExploding)
	{
		return true;
	}
	int i;
	for (i = 0; i < NUM_BULLETS; i++)
	{
		if (bullets[i]->IsActive())
		{
			return true;
		}
	}
	return false; 
}

void Grenade::SetIsExploding(bool value)
{
	if (value && !isExploding)
	{
		isExploding = value;
		justExploded = true;

		for (int i = 0; i < NUM_BULLETS; i++)
			bullets[i]->SetIsMoving(value);
	}
	else if (!value)
	{
		isExploding = value;
	}
}


void Grenade::CreateSecurityMap(const double* pMap, double smap[MSX][MSY])
{
	int i;
	for (i = 0; i < NUM_BULLETS; i++)
		bullets[i]->CreateSecurityMap(pMap, smap); 

}