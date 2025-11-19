#pragma once

// IPathfinding.h - Interface for A* pathfinding

#include "Definition.h" // Includes global definitions
#include "Cell.h" // Defines the A* node structure
#include <list>
#include <map>
#include <vector>
#include <queue>

struct CompareCells {
	bool operator()(const Cell* a, const Cell* b) const {
		return a->getF() > b->getF(); // Orders by F-cost (smaller F means higher priority).
	}
};

// costs
const double SAFETY_WEIGHT = 10.0; // risk penalty
const double STRAIGHT_COST = 1.0; // Base cost for a single move step (G-cost increment)

const double SAFETY_THRESHOLD = 1.5; // Threshold for a map cell to be considered 'safe'

// abstract class for pathfinding
class IPathfinding
{
protected:
	// path storage
	std::list<Point> currentPath; // stores the calculated path as a list of coordinates.
	std::vector<Cell*> createdCells; // storage for all created cells

	// helper methods
	bool IsGoalReached(const Point& current, const Point& goal) const;
	// Calculates the cost of moving to a neighbor
	double CalculateStepCost(int dx, int dy, const Point& neighbor, const double* safetyMap) const;
	
	// Handles evaluation and update of a neighbor cell in A*.
	void ProcessNeighbors(Cell* current, const Point& neighbor, const Point& goal,
		const double* safetyMap,
		std::map<int, Cell*>& allCells,
		std::vector<Cell*>& createdCells,
		std::priority_queue<Cell*, std::vector<Cell*>, CompareCells>& openList);

	void CleanupCells(std::vector<Cell*>& createdCells);

	// Reconstructs the final path by backtracking from the goal cell.
	void RestorePath(Cell* goalCell, std::list<Point>& path);

	bool IsWalkable(const double* safetyMap, Point p) const;
	// virtual methods implemented by characters
	virtual Point GetLocation() const = 0; // gets the current grid location of the character

public:
	virtual ~IPathfinding() = default; // destructor

	// interface methods:

	// Attempts to find the lowest-cost path to a goal using A*.
	virtual bool FindAStarPath(Point goal, const double* safetyMap);
	
	// Finds the nearest safe position within a range using BFS.
	virtual Point FindClosestSafePosition(double searchRange, const double* safetyMap) const;

	// getters
	const std::list<Point>& GetCurrentPath() const { return currentPath; }
	void ClearPath() { currentPath.clear(); }
	bool HasPath() const { return !currentPath.empty(); }
};