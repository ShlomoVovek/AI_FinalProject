#pragma once

#include "glut.h"
#include "Definition.h"
#include <vector>
#include <set>

class Map 
{
private: 
	int mapWidth;
	int mapHeight;
	std::set<std::pair<int, int>> agentPositions;

	// for saving the random coordinates
	std::vector<Point> trees;
	std::vector<Point> rocks;
	std::vector<Point> waterBlocks;
	std::vector<Point> warehouses;
	
	void DrawForest();
	void DrawRocks();
	void DrawWater();
	void DrawWarehouses();

	void GenerateRandomElements();

	// check if cell is already occupoied by character or obsacle
	bool IsOccupied(int x, int y) const;

public:
	// constructor
	Map(int w = MSX, int h = MSY);
	const int BOUNDARY_MARGIN = 3;

	double gameMapData[MSX][MSY];

	void ClearAgents() { agentPositions.clear(); }
	void DrawField();

	// getters
	const std::vector<Point>& GetTrees() const { return trees; }
	const std::vector<Point>& GetRocks() const { return rocks; }
	const std::vector<Point>& GetWater() const { return waterBlocks; }
	const std::vector<Point>& GetWarehouses() const { return warehouses; }

	// point info
	bool IsStaticObstacle(int x, int y) const;
	bool IsAgentAt(int x, int y) const { return agentPositions.count({ x, y }) > 0; }
	bool IsWalkable(int x, int y) const;

	void RegisterNewAgent(int x, int y);
	void RegisterAgentDeath(int x, int y);
	void RegisterAgentMove(int oldX, int oldY, int newX, int newY);

	// returns what this cell contains
	int CheckPositionContent(int x, int y) const
	{
		if (x >= 0 && x < MSX && y >= 0 && y < MSY)
			return (int)gameMapData[x][y];
		return EMPTY;
	}
};