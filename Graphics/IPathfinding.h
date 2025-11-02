#pragma once

// IPathfinding.h - Interface for A* pathfinding

#include "Definition.h"
#include "Cell.h"
#include <list>
#include <map>
#include <vector>
#include <queue>

struct CompareCells {
	bool operator()(const Cell* a, const Cell* b) const {
		return a->getF() > b->getF();
	}
};

// costs
const double SAFETY_WEIGHT = 10.0;
// const double DIAGONAL_COST = 1.414; // squre root of 2 
const double STRAIGHT_COST = 1.0;

const double SAFETY_THRESHOLD = 0.5;

// abstract class for pathfinding
class IPathfinding
{
protected:
	// path storage
	std::list<Point> currentPath;
	std::vector<Cell*> createdCells; // storage for all created cells

	// helper methods
	bool IsGoalReached(const Point& current, const Point& goal) const;
	double CalculateStepCost(int dx, int dy, const Point& neighbor, const double* safetyMap) const;
	
	void ProcessNeighbors(Cell* current, const Point& neighbor, const Point& goal,
		const double* safetyMap,
		std::map<int, Cell*>& allCells,
		std::vector<Cell*>& createdCells,
		std::priority_queue<Cell*, std::vector<Cell*>, CompareCells>& openList);

	void CleanupCells(std::vector<Cell*>& createdCells);
	void RestorePath(Cell* goalCell, std::list<Point>& path);

	bool IsWalkable(const double* safetyMap, Point p) const;
	// virtual methods implemented by characters
	virtual Point GetLoaction() const = 0;

public:
	virtual ~IPathfinding() = default; // destructor

	// interface methods
	virtual bool FindAStarPath(Point goal, const double* safetyMap);
	// must use BFS
	virtual Point FindClosestSafePosition(double searchRange, const double* safetyMap) const;

	// getters
	const std::list<Point>& GetCurrentPath() const { return currentPath; }
	void ClearPath() { currentPath.clear(); }
	bool HasPath() const { return !currentPath.empty(); }
};