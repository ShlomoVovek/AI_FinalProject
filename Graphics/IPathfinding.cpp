#include "IPathfinding.h"

extern const int MSX;
extern const int MSY;

bool IPathfinding::IsWalkable(const double* safetyMap, Point p) const
{
	// check if out of bounderies
	if (0 > p.x || p.x >= MSX || 0 > p.y || p.y >= MSY || safetyMap == nullptr)
		return false;

	int index = (int)p.x * MSY + (int)p.y;
	if (index < 0 || index >= MSX * MSY) return false;
	
	double cellValue = safetyMap[index];
	if (cellValue == (double)ROCK || cellValue == (double)WATER)
		return false;

	return true; // is not Water or Rock
}

// calculate the cost of making the next step:
//		adding the basic cost of stepping forward with the safety penalty
double IPathfinding::CalculateStepCost(int dx, int dy, const Point& neighbor, const double* safetyMap) const
{
	if (neighbor.x < 0 || neighbor.x >= MSX || neighbor.y < 0 || neighbor.y >= MSY)
		return 9999.0; // high costs
	
	int index = (int)neighbor.x * MSY + (int)neighbor.y;

	// check bounderies
	if (index < 0 || index >= MSX * MSY)
		return 9999.0; // high costs

	// 3. calculate safety penalty
	double safetyValue = safetyMap[index];
	double safetyPenalty = 0.0;
	
	if (safetyValue >= 0)
	{
		safetyPenalty = safetyValue * SAFETY_WEIGHT;
	}

	// 4. sum sosts
	return STRAIGHT_COST + safetyPenalty;
}

bool IPathfinding::IsGoalReached(const Point& current, const Point& goal) const
{
	return (current.x == goal.x) && (current.y == goal.y);
}

// calculate H vlaue with Manhattan distance
double CalculateHeuristic(const Point& current, const Point& goal)
{
	return (double)ManhattanDistance(current, goal);
}

void IPathfinding::ProcessNeighbors(Cell* current, const Point& neighbor, const Point& goal,
	const double* safetyMap,
	std::map<int, Cell*>& allCells,
	std::vector<Cell*>& createdCells,
	std::priority_queue<Cell*, std::vector<Cell*>, CompareCells>& openList)
{
	int neighborIndex = (int)neighbor.x * MSY + (int)neighbor.y;

	// 1. lculate new G value costs
	int dx = neighbor.x - current->getPoint().x;
	int dy = neighbor.y - current->getPoint().y;

	double stepCost = CalculateStepCost(dx, dy, neighbor, safetyMap);
	double newG = current->getG() + stepCost;
	
	Cell* neighborCell = nullptr;

	// 2. check if cell already created: if in openList or allCells
	auto it = allCells.find(neighborIndex);
	if (it != allCells.end())
	{
		neighborCell = it->second;

		// 3. cell already exists -> check if new path is better path
		if (newG < neighborCell->getG())
		{
			// found better, update data
			neighborCell->setG(newG);
			neighborCell->setParent(current);
			neighborCell->setF(newG + neighborCell->getH());
		
			openList.push(neighborCell); // can not change priority of exist element in priority_queue
										 // the worse element will be handled when exit the queue
		}
	}
	else // 4. create new cell and add to openList
	{
		double h = CalculateHeuristic(neighbor, goal);

		neighborCell = new Cell(neighbor, newG, h, current);
		neighborCell->setF(newG + h);

		allCells[neighborIndex] = neighborCell;
		createdCells.push_back(neighborCell);
		openList.push(neighborCell);
	}
}

Point IPathfinding::FindClosestSafePosition(double searchRange, const double* safetyMap) const
{
	Point start = GetLocation();

	if (safetyMap == nullptr || MSX <= 0 || MSY <= 0)
		return start;

	std::queue<Point> q;
	std::map<int, bool> visited;

	q.push(start);
	visited[(int)start.x * MSY + (int)start.y] = true;

	while (!q.empty())
	{
		Point current = q.front();
		q.pop();

		int index = (int)current.x * MSY + (int)current.y;
		if (index >= 0 && index < MSX * MSY)
		{
			if (safetyMap[index] <= SAFETY_THRESHOLD)
			{
				return current;
			}
		}
		// check neighbors
		for (int dy = -1; dy <= 1; ++dy)
			for (int dx = -1; dx <= 1; ++dx)
			{
				if (dx == 0 && dy == 0) continue; // skip current location

				Point neighbor = { current.x + dx, current.y + dy };
				int neighborIndex = (int)neighbor.x * MSY + (int)neighbor.y;

				// 1. check boundaries
				if (neighbor.x < 0 || neighbor.x >= MSX || neighbor.y < 0 || neighbor.y >= MSY)
					continue;

				// 2. is visited
				if (visited[neighborIndex]) continue;

				// 3. Walkability
				if (IsWalkable(safetyMap, neighbor))
				{
					// Use Manhattan distance for search range
					double manhattanDist = ManhattanDistance(start, neighbor);

					if (manhattanDist <= searchRange)
					{
						visited[neighborIndex] = true;
						q.push(neighbor);
					}
				}
			}
	}
	return start;
}

bool IPathfinding::FindAStarPath(Point goal, const double* safetyMap)
{
	currentPath.clear();
	CleanupCells(createdCells);

	if (!IsWalkable(safetyMap, goal))
	{
		// if not walkable, try finding other
		bool foundAlternative = false;
		for (int dy = -1; dy <= 1 && !foundAlternative; ++dy)
		{
			for (int dx = -1; dx <= 1 && !foundAlternative; ++dx)
			{
				if (dx == 0 && dy == 0) continue;

				Point altGoal = { goal.x + dx, goal.y + dy };
				if (altGoal.x >= 0 && altGoal.x < MSX &&
					altGoal.y >= 0 && altGoal.y < MSY &&
					IsWalkable(safetyMap, altGoal))
				{
					goal = altGoal;
					foundAlternative = true;
				}
			}
		}

		if (!foundAlternative)
			return false;
	}

	std::map<int, Cell*> allCells;
	std::priority_queue<Cell*, std::vector<Cell*>, CompareCells> openList;
	std::map<int, bool> closedList;

	// 1. reset
	Point start = GetLocation();

	if (start.x == goal.x && start.y == goal.y)
	{
		return true;
	}

	// start: g = 0, h = Heuristic, f = h
	double hStart = CalculateHeuristic(start, goal);
	Cell* startCell = new Cell(start, 0.0, hStart, nullptr);
	startCell->setF(hStart);

	int startIndex = (int)start.x * MSY + (int)start.y;
	allCells[startIndex] = startCell;
	createdCells.push_back(startCell);
	openList.push(startCell);

	Cell* goalCell = nullptr;

	// set Goal Point
	const Point goalPoint = goal;
	int iterations = 0;
	const int MAX_ITERATIONS = MSX * MSY;

	// 2. main search loop
	while (!openList.empty() && iterations < MAX_ITERATIONS)
	{
		iterations++;

		Cell* current = openList.top();
		openList.pop();

		int currentIndex = current->getPoint().x * MSY + current->getPoint().y;

		if (closedList[currentIndex])
			continue;

		closedList[currentIndex] = true;

		if (IsGoalReached(current->getPoint(), goal))
		{
			goalCell = current;
			break;
		}

		for (int dy = -1; dy <= 1; ++dy)
		{
			for (int dx = -1; dx <= 1; ++dx)
			{
				if (dy == 0 && dx == 0)
					continue;

				Point neighbor = { current->getPoint().x + dx, current->getPoint().y + dy };

				if (neighbor.x >= 0 && neighbor.x < MSX &&
					neighbor.y >= 0 && neighbor.y < MSY)
				{
					int neighborIndex = neighbor.x * MSY + neighbor.y;

					if (closedList[neighborIndex])
						continue;

					if (IsWalkable(safetyMap, neighbor))
					{
						ProcessNeighbors(current, neighbor, goal, safetyMap,
							allCells, createdCells, openList);
					}
				}
			}
		}
	}

	// 5. cleanup memory
	if (goalCell != nullptr)
	{
		RestorePath(goalCell, currentPath);
	}
	// CleanupCells(createdCells);

	// 7. Return true if path was found
	return (goalCell != nullptr && !currentPath.empty());
}

void IPathfinding::CleanupCells(std::vector<Cell*>& createdCells)
{
	for (Cell* cell : createdCells)
	{
		delete cell;
	}
	createdCells.clear();
}

void IPathfinding::RestorePath(Cell* goalCell, std::list<Point>& path)
{
	path.clear();
	Cell* current = goalCell;
	while (current != nullptr)
	{
		path.push_front(current->getPoint());
		current = current->getParent();
	}
	if (!path.empty())
	{
		path.pop_front();
	}
}























