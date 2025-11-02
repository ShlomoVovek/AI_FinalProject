#pragma once
#include "glut.h"
#include "Map.h"
#include "Definition.h"
#include "Cell.h"
#include <vector>
#include <list>


const double SPEED = 0.5;
const double MAX_HP = 100.0;
const double CRITICAL_HP = MAX_HP * 0.25;

enum TeamColor
{
	TEAM_RED,
	TEAM_BLUE
};

enum NpcType
{
	COMMANDER = 'C',
	WARRIOR = 'W',
	MEDIC = 'M',
	SUPPLIER = 'S'
};

enum CommandType
{
	CMD_NONE,
	CMD_MOVE,      
	CMD_ATTACK,    
	CMD_DEFEND,    
	CMD_RESUPPLY,  
	CMD_HEAL	   
};

struct Sighting // report seeing enemey
{
	NpcType type;
	Point point;
};

class Commander; // forward declaration for myCommander pointer attribute

class NPC // abstract class
{
protected:
	Point location;
	TeamColor team;
	NpcType type;
	Commander* myCommander;
	double health;

	// behavior properties
	Point targetLocation;
	double directionX, directionY;
	bool isMoving;

	double viewMap[MSX][MSY];

	// drawing methods
	void DrawNpcBase() const;
	void DrawLetter() const;
	void DrawHealthBar() const;

	// double Distance(double x1, double y1, double x2, double y2);
	void SetDirection(Point  target);
	void MoveToTarget();
	// bool IsWalkable(Point p) const; implemented in IPathFinding

	// build NPC's own view map by obstacles from the main map of the game.
	// each NPC has limited SIGHT_RANGE, and other obstacles so it might not see
	// the whole map.
	void BuildViewMap(const double* pMap); 

	virtual void CalculatePathAndMove() = 0;

public:
	// constructor
	NPC(int x, int y, TeamColor t, NpcType nt);

	void SetCommander(Commander* pCommander);
	virtual void Show() const;

	// update loop, called in every frame by idle():
	// checks currentState value and do NPC's work
	virtual void DoSomeWork(const double* pMap) = 0;
		

	// status and HP
	bool IsAlive() const { return health > 0; }
	bool CanFight() const { return health > CRITICAL_HP; }

	// report methods
	virtual void ReportInjury(NPC* injuredSoldier) = 0;
	virtual void ReportLowAmmo(NPC* warrior) = 0;


	void TakeDamage(double dmg) { health -= dmg; }
	void heal(double amount) { health = (health + amount > MAX_HP) ? MAX_HP : health + amount; }

	// getters
	Point GetLocation() const { return location;}
	NpcType GetType() const { return type; }
	TeamColor GetTeam() const { return team; }
	const double* GetViewMap() const { return (const double*) viewMap; }

	// commander methods
	virtual void ReportSighting(NpcType enemyType, Point enemyLoc) = 0;
	
	// Event Handler method type:
	/*
		Called once when Commander send new COMMAND_CODE.
		It stops current command and reset currentState value,
		as well as call FindAStartPath().
	
	*/
	virtual void ExecuteCommand(int commandCode, Point target) = 0; 

};

