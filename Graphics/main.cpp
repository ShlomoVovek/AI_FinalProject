
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <set>
#include <chrono>
#include <thread>
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

bool isGameOver = false;

void InitCharacters()
{
	isGameOver = false;

	// reset characters
	for (NPC* agent : allAgents)
		delete agent;
	allAgents.clear();
	allWarriors.clear();

	if (gameMap != nullptr)
		gameMap->ClearAgents();
	else
	{
		printf("ERROR: gameMap is null during InitCharacters\n");
		exit(1);
	}

	auto findRandomPosition = [&](TeamColor team) -> Point
	{
		int maxAttempts = 500;
		int attempt = 0;

		while (attempt < maxAttempts)
		{
			int x, y;

			if (team == TEAM_RED)
				x = 5 + rand() % (MSX / 2 - 10);

			else // TEAM_BLUE
				x = (MSX / 2 + 5) + rand() % (MSX / 2 - 10);

			y = 5 + rand() % (MSY - 10); // Y can be anywhere

			// Check if position is free:
			int obstacleType = gameMap->CheckPositionContent(x, y);

			if ((obstacleType == EMPTY || obstacleType == TREE) &&
				!gameMap->IsAgentAt(x, y))
			{
				obstacleType = gameMap->CheckPositionContent(x, y);
				if (obstacleType == EMPTY || obstacleType == TREE)
				{
					return { (int)x, (int)y };
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

	// --- RED Team ---
	pos = findRandomPosition(TEAM_RED);
	NPC* newCommanderR = new Commander((int)pos.x, (int)pos.y, TEAM_RED);
	newCommanderR->SetMap(gameMap);
	gameMap->RegisterNewAgent((int)pos.x, (int)pos.y);
	allAgents.push_back(newCommanderR);

	pos = findRandomPosition(TEAM_RED);
	Warrior* redWarrior1 = new Warrior((int)pos.x, (int)pos.y, TEAM_RED);
	redWarrior1->SetMap(gameMap);
	gameMap->RegisterNewAgent((int)pos.x, (int)pos.y);
	allAgents.push_back(redWarrior1);
	allWarriors.push_back(redWarrior1);

	pos = findRandomPosition(TEAM_RED);
	Warrior* redWarrior2 = new Warrior((int)pos.x, (int)pos.y, TEAM_RED);
	redWarrior2->SetMap(gameMap);
	gameMap->RegisterNewAgent((int)pos.x, (int)pos.y);
	allAgents.push_back(redWarrior2);
	allWarriors.push_back(redWarrior2);

	pos = findRandomPosition(TEAM_RED);
	NPC* newMedicR = new Medic((int)pos.x, (int)pos.y, TEAM_RED);
	newMedicR->SetMap(gameMap);
	gameMap->RegisterNewAgent((int)pos.x, (int)pos.y);
	allAgents.push_back(newMedicR);

	pos = findRandomPosition(TEAM_RED);
	NPC* newSupplierR = new SupplyAgent((int)pos.x, (int)pos.y, TEAM_RED);
	newSupplierR->SetMap(gameMap);
	gameMap->RegisterNewAgent((int)pos.x, (int)pos.y);
	allAgents.push_back(newSupplierR);

	// --- BLUE Team ---
	pos = findRandomPosition(TEAM_BLUE);
	NPC* newCommanderB = new Commander((int)pos.x, (int)pos.y, TEAM_BLUE);
	newCommanderB->SetMap(gameMap);
	gameMap->RegisterNewAgent((int)pos.x, (int)pos.y);
	allAgents.push_back(newCommanderB);

	pos = findRandomPosition(TEAM_BLUE);
	Warrior* blueWarrior1 = new Warrior((int)pos.x, (int)pos.y, TEAM_BLUE);
	blueWarrior1->SetMap(gameMap);
	gameMap->RegisterNewAgent((int)pos.x, (int)pos.y);
	allAgents.push_back(blueWarrior1);
	allWarriors.push_back(blueWarrior1);

	pos = findRandomPosition(TEAM_BLUE);
	Warrior* blueWarrior2 = new Warrior((int)pos.x, (int)pos.y, TEAM_BLUE);
	blueWarrior2->SetMap(gameMap);
	gameMap->RegisterNewAgent((int)pos.x, (int)pos.y);
	allAgents.push_back(blueWarrior2);
	allWarriors.push_back(blueWarrior2);

	pos = findRandomPosition(TEAM_BLUE);
	NPC* newMedicB = new Medic((int)pos.x, (int)pos.y, TEAM_BLUE);
	newMedicB->SetMap(gameMap);
	gameMap->RegisterNewAgent((int)pos.x, (int)pos.y);
	allAgents.push_back(newMedicB);

	pos = findRandomPosition(TEAM_BLUE);
	NPC* newSupplierB = new SupplyAgent((int)pos.x, (int)pos.y, TEAM_BLUE);
	newSupplierB->SetMap(gameMap);
	gameMap->RegisterNewAgent((int)pos.x, (int)pos.y);
	allAgents.push_back(newSupplierB);

	// connect soldier to Commander
	Commander* redCommander = nullptr;
	Commander* blueCommander = nullptr;

	// TODO: call for another helper method, instead of this long one
	
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
		agent->SetNpcList(&allAgents);
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

	ShowMaze();

	glutSwapBuffers(); // show all
}
*/

/*
void displayCommanderMap()
{
	glClear(GL_COLOR_BUFFER_BIT); // clean frame buffer

	ShowMaze();

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

	for (NPC* agent : allAgents)
		agent->Show();

	glutSwapBuffers(); // show all
}

void idle() 
{
	if (gameMap == nullptr)
		return;

	if (isGameOver)
	{
		glutPostRedisplay();
		Sleep(50);
		return;
	}

	// check if game is over
	int livingRedAgents = 0;
	int livingBlueAgents = 0;

	for (NPC* agent : allAgents)
	{
		if (agent->IsAlive())
		{
			if (agent->GetTeam() == TEAM_RED)
				livingRedAgents++;
			else if (agent->GetTeam() == TEAM_BLUE)
				livingBlueAgents++;
		}
	}

	if ((livingRedAgents == 0 || livingBlueAgents == 0) && !allAgents.empty())
	{
		isGameOver = true;

		if (livingRedAgents == 0 && livingBlueAgents > 0)
		{
			printf("====================================\n");
			printf("All RED agents are down. BLUE team wins!\n");
			printf("====================================\n");
		}
		else if (livingBlueAgents == 0 && livingRedAgents > 0)
		{
			printf("=====================================\n");
			printf("All BLUE agents are down. RED team wins!\n");
			printf("=====================================\n");
		}
		else if (livingRedAgents == 0 && livingBlueAgents == 0)
		{
			printf("=================================\n");
			printf("It's a draw! All agents are down.\n");
			printf("=================================\n");
		}

		printf("Game Over. Press 'r' to restart.\n");
	}
	
	const double* pMapData = (const double*)gameMap->gameMapData;

	for (NPC* agent : allAgents)
	{
		agent->DoSomeWork(pMapData);
		
	}

	glutPostRedisplay();
	Sleep(50);
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'r':
	case 'R':
	{
		printf("Restarting game...\n");
		InitCharacters();
		glutIdleFunc(idle);
	}
		break;
	case 27: // ESC key
		exit(0); // Exit game
		break;
	}
}

int main(int argc, char* argv[]) 
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(1200, 720);
	glutInitWindowPosition(250, 50);
	glutCreateWindow("Press 'R' to restart the simulation, Press Esc for exit");

	glutDisplayFunc(display);
	//glutDisplayFunc(displayGameMap);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);

	// TODO: glutReshapeFunc(reshape);

	// glutCreateMenu(viewMenu);
	glutAddMenuEntry("Game Map", 1);
	glutAddMenuEntry("Commander Visibility Map", 2);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	init();

	glutMainLoop();

	return 0;
}