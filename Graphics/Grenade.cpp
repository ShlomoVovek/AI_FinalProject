#include "Grenade.h"

Grenade::Grenade(double posX, double posY, TeamColor grenadeTeam) :
	x(posX), y(posY), isExploding(false), team(grenadeTeam)
{
	int i;
	double alpha, teta = 2 * PI / NUM_BULLETS;

	// x = posX; // (כבר מבוצע ברשימת האתחול)
	// y = posY; // (כבר מבוצע ברשימת האתחול)

	for (i = 0, alpha = 0; i < NUM_BULLETS; i++, alpha += teta)
	{
		// העבר את ה-team לבנאי של Bullet
		bullets[i] = new Bullet(x, y, alpha, team);
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
	int i;
	for (i = 0; i < NUM_BULLETS; i++)
		bullets[i]->Show();
}

void Grenade::Explode(const double* pMap, const std::vector<NPC*>& npcs)
{
	int i;
	for (i = 0; i < NUM_BULLETS; i++)
		bullets[i]->Move(pMap, npcs); 

}

bool Grenade::IsActive()
{
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
	isExploding = value;
	int i;
	for (i = 0; i < NUM_BULLETS; i++)
		bullets[i]->SetIsMoving(value);
}

// vvv שינוי במימוש הפונקציה vvv
void Grenade::CreateSecurityMap(const double* pMap, double smap[MSX][MSY])
{
	int i;
	for (i = 0; i < NUM_BULLETS; i++)
		bullets[i]->CreateSecurityMap(pMap, smap); // קריאה לפונקציה המעודכנת

}