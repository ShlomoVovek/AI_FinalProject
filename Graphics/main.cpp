
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <set>
#include "glut.h"
#include "NPC.h"
#include "Commander.h"
#include "Warrior.h"
#include "Medic.h"
#include "SupplyAgent.h"
#include "Map.h"

extern const int MSX;
extern const int MSY;

const double ASPECT_RATIO = (double)MSX / MSY;
int id = 0;
Map* gameMap = nullptr;
std::vector<NPC*> allAgents;
std::vector<Warrior*> allWarriors;

void InitCharacters()
{
	// reset characters
	for (NPC* agent : allAgents)
		delete agent;
	allAgents.clear();
	allWarriors.clear();

	// Store all occupied locations both obstacles & characters
	std::set<std::pair<int, int>> occupiedPositions;

	auto findRandomPosition = [&](TeamColor team) -> Point
	{
		int maxAttempts = 500;
		int attempt = 0;

		while (attempt < maxAttempts)
		{
			int x, y;
			// Red team: left half (x from 5 to MSX/2 - 5)
			// Blue team: right half (x from MSX/2 + 5 to MSX - 5)

			if (team == TEAM_RED)
				x = 5 + rand() % (MSX / 2 - 10);

			else // TEAM_BLUE
				x = (MSX / 2 + 5) + rand() % (MSX / 2 - 10);

			y = 5 + rand() % (MSY - 10); // Y can be anywhere

			// Check if position is free:
			std::pair<int, int> pos = { x, y };

			if (occupiedPositions.find(pos) == occupiedPositions.end())
			{
				// TODO: handle NULL position
				CellType obstacleType = (CellType)gameMap->CheckPositionContent(x, y);

				if (gameMap) // is not null
				{
					// Rule 1: Can spawn on EMPTY tiles
					// Rule 2: Can spawn on TREE tiles
					// Rule 3: CANNOT spawn on ROCK, WATER, or BASE
					if (obstacleType == EMPTY || obstacleType == TREE)
					{
						occupiedPositions.insert(pos);
						return { (int)x, (int)y };
					}
				}
			}
			attempt++;
		};

		if (attempt >= maxAttempts)
		{
			printf("ERROR: Could not find a valid spawn position\n");
			exit(1);
		}
		return { 0, 0 };
	};
	Point pos;

	pos = findRandomPosition(TEAM_RED);
	allAgents.push_back(new Commander((int)pos.x, (int)pos.y, TEAM_RED));

	pos = findRandomPosition(TEAM_RED);
	Warrior* redWarrior1 = new Warrior((int)pos.x, (int)pos.y, TEAM_RED);
	allAgents.push_back(redWarrior1);
	allWarriors.push_back(redWarrior1);

	pos = findRandomPosition(TEAM_RED);
	Warrior* redWarrior2 = new Warrior((int)pos.x, (int)pos.y, TEAM_RED);
	allAgents.push_back(redWarrior2);
	allWarriors.push_back(redWarrior2);

	pos = findRandomPosition(TEAM_RED);
	allAgents.push_back(new Medic((int)pos.x, (int)pos.y, TEAM_RED));

	pos = findRandomPosition(TEAM_RED);
	allAgents.push_back(new SupplyAgent((int)pos.x, (int)pos.y, TEAM_RED));

	// BLUE team creation
	pos = findRandomPosition(TEAM_BLUE);
	allAgents.push_back(new Commander((int)pos.x, (int)pos.y, TEAM_BLUE));

	pos = findRandomPosition(TEAM_BLUE);
	Warrior* blueWarrior1 = new Warrior((int)pos.x, (int)pos.y, TEAM_BLUE);
	allAgents.push_back(blueWarrior1);
	allWarriors.push_back(blueWarrior1);

	pos = findRandomPosition(TEAM_BLUE);
	Warrior* blueWarrior2 = new Warrior((int)pos.x, (int)pos.y, TEAM_BLUE);
	allAgents.push_back(blueWarrior2);
	allWarriors.push_back(blueWarrior2);

	pos = findRandomPosition(TEAM_BLUE);
	allAgents.push_back(new Medic((int)pos.x, (int)pos.y, TEAM_BLUE));

	pos = findRandomPosition(TEAM_BLUE);
	allAgents.push_back(new SupplyAgent((int)pos.x, (int)pos.y, TEAM_BLUE));

	// connect soldier to Commander
	Commander* redCommander = nullptr;
	Commander* blueCommander = nullptr;

	// TODO: call for another helper method, 
	// TODO: populate the Commander* ptr in creation
	
	// find Commander
	for (NPC* agent : allAgents)
	{
		if (agent->GetType() == COMMANDER)
		{
			if (agent->GetTeam() == TEAM_RED)
				redCommander = dynamic_cast<Commander*>(agent);
			else
				blueCommander = dynamic_cast<Commander*>(agent);
		}
	}
	// connect to the right color
	for (NPC* agent : allAgents)
	{
		if (agent->GetType() != COMMANDER)
		{
			if (agent->GetTeam() == TEAM_RED && redCommander)
			{
				redCommander->AddMemeber(agent);
				agent->SetCommander(redCommander);
			}
			else if (agent->GetTeam() == TEAM_BLUE && blueCommander)
			{
				blueCommander->AddMemeber(agent);
				agent->SetCommander(blueCommander);
			}
		}
	}
}

void init()
{
	srand(time(nullptr));
	glClearColor(0.5, 0.8, 0.5, 0); // color of window background
	glOrtho(0, MSX, 0, MSY, -1, 1); // set the coordinates system

	// 1. create map
	gameMap = new Map(MSX, MSY);

	// 2. initialize characters
	InitCharacters();
}

/*
void displayGameMap()
{
	glClear(GL_COLOR_BUFFER_BIT); // clean frame buffer

	ShowMaze(); // This draws your regular maze

	glutSwapBuffers(); // show all
}
*/

/*
void displayCommanderMap()
{
	glClear(GL_COLOR_BUFFER_BIT); // clean frame buffer

	// --- TODO: Add your logic here ---
	// You probably want to draw the base map first
	ShowMaze();

	// Then, draw the visibility overlay on top of it
	// For example:
	// DrawCommanderVisibilityOverlay();

	glutSwapBuffers(); // show all
}
*/

/*
// Menu callback function to switch views
void viewMenu(int choice)
{
	switch (choice)
	{
	case 1:  // Game Map
		glutDisplayFunc(displayGameMap); // Set display function to show Game Map
		break;
	case 2:   // Commander Visibility Map
		glutDisplayFunc(displayCommanderMap); // Set display function to show Commander Map
		break;
	}
	glutPostRedisplay(); // Request a redraw to show the change
}
*/

void display()
{
	glClear(GL_COLOR_BUFFER_BIT); // clean frame buffer

	if (gameMap)
		gameMap->DrawField();

	// draw all characters (this will now include the shot lines for warriors)
	for (NPC* agent : allAgents)
		agent->Show();

	glutSwapBuffers(); // show all
}

void idle() 
{
	if (gameMap == nullptr)
		return;
	
	const double* pMapData = (const double*)gameMap->gameMapData;

	for (NPC* agent : allAgents)
	{
		agent->DoSomeWork(pMapData);
		
	}
	printf("id = %d\n", id++);
	glutPostRedisplay();
}

void main(int argc, char* argv[]) 
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(1200, 720);
	glutInitWindowPosition(250, 50);
	glutCreateWindow("AI Final Project");

	glutDisplayFunc(display);
	//glutDisplayFunc(displayGameMap);
	glutIdleFunc(idle);

	//glutKeyboardFunc(keyboard);

	// TODO: glutReshapeFunc(reshape);

	// glutCreateMenu(viewMenu);
	glutAddMenuEntry("Game Map", 1);
	glutAddMenuEntry("Commander Visibility Map", 2);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	init();

	glutMainLoop();
}