#include "Map.h"
#include <stdlib.h>
#include <time.h>
#include <math.h>

Map::Map(int w, int h) : mapWidth(w), mapHeight(h)
{
    srand(time(nullptr));
    
    for (int i = 0; i < MSX; i++)
        for (int j = 0; j < MSY; j++)
        {
            gameMapData[i][j] = EMPTY;
        }

    GenerateRandomElements();
}

bool Map::IsOccupied(int x, int y) const 
{
    // Check trees
    for (const auto& p : trees)
        if ((int)p.x == x && (int)p.y == y) return true;
    // Check rocks
    for (const auto& p : rocks)
        if ((int)p.x == x && (int)p.y == y) return true;
    // Check water
    for (const auto& p : waterBlocks)
        if ((int)p.x == x && (int)p.y == y) return true;
    // Check warehouses
    for (const auto& p : warehouses)
        if ((int)p.x == x && (int)p.y == y) return true;

    return false;
}

// map info and position managment
bool Map::IsStaticObstacle(int x, int y) const
{
    if (x < 0 || x >= mapWidth || y < 0 || y >= mapHeight)
        return true;

    int type = gameMapData[x][y];
    return (type == ROCK || type == WATER); 
}

bool Map::IsWalkable(int x, int y) const
{
    return !IsStaticObstacle(x, y) && !IsAgentAt(x, y);
}

void Map::RegisterNewAgent(int x, int y)
{
    agentPositions.insert({ x, y });
}

void Map::RegisterAgentDeath(int x, int y)
{
    agentPositions.erase({ x, y });
}

void Map::RegisterAgentMove(int oldX, int oldY, int newX, int newY)
{
    agentPositions.erase({ oldX, oldY });
    agentPositions.insert({ newX, newY });
}

void Map::GenerateRandomElements()
{
    int numTrees = (int)(MSX / 5 + rand() % 10);
    int numRocks = (int)(MSX / 5 + rand() % 5);
    int numWater = (int)(MSX / 5 + + rand() % 5);
    // int numWarehouses = 4;

    auto generatePoint = [this]() -> Point // random coordinates
        {
            return
            {
                (int)(BOUNDARY_MARGIN + (rand() % (this->mapWidth - 2 * BOUNDARY_MARGIN))),
                (int)(BOUNDARY_MARGIN + (rand() % (this->mapHeight - 2 * BOUNDARY_MARGIN)))
            };
        };

    Point p;

    for (int i = 0; i < numTrees; i++)
    {
        p = generatePoint();
        trees.push_back(p);
        gameMapData[(int)p.x][(int)p.y] = TREE;
    }

    for (int i = 0; i < numRocks; i++)
    {
        do {
            p = generatePoint();
        } while (IsOccupied((int)p.x, (int)p.y));
        rocks.push_back(p);
        gameMapData[(int)p.x][(int)p.y] = ROCK;
    }

    for (int i = 0; i < numWater; i++)
    {
        do {
            p = generatePoint();
        } while (IsOccupied((int)p.x, (int)p.y));
        waterBlocks.push_back(p);
        gameMapData[(int)p.x][(int)p.y] = WATER;
    }

 // warehaouses

    // left on X axis
    int minX_Left = 4;
    int maxX_Left = (int)(mapWidth * 0.25) - 1;
    int rangeX_Left = maxX_Left - minX_Left + 1;

    // right on X axis
    int minX_Right = (int)(mapWidth * 0.75) + 1;
    int maxX_Right = mapWidth - 4;
    int rangeX_Right = maxX_Right - minX_Right + 1;

    // Y axis
    int minY = 4;
    int maxY = mapHeight - 1;
    int rangeY = maxY - minY + 1;

    for (int i = 0; i < 2; ++i)
    {
        do {
            p.x = minX_Left + (rand() % rangeX_Left);
            p.y = minY + (rand() % rangeY);
        } while (IsOccupied((int)p.x, (int)p.y));
        warehouses.push_back(p);
        gameMapData[(int)p.x][(int)p.y] = RED_BASE;
    }

    for (int i = 0; i < 2; ++i)
    {
        do {
            p.x = minX_Right + (rand() % rangeX_Right);
            p.y = minY + (rand() % rangeY);
        } while (IsOccupied((int)p.x, (int)p.y));
        warehouses.push_back(p);
        gameMapData[(int)p.x][(int)p.y] = BLUE_BASE;
    }
}

void Map::DrawForest()
{
    double treeSize = 0.5;

    for (const auto& p : trees)
    {
        // TODO: rearrange trees
        // --- Draw the bottom, wider triangle ---
        glBegin(GL_POLYGON);
        glColor3d(0, 0.5, 0);
        // Top-center point (slightly above center)
        glVertex2d(p.x, p.y + (treeSize * 0.4)); // (p.x, p.y + 0.2)
        // Bottom-left corner
        glVertex2d(p.x - treeSize, p.y - treeSize); // (p.x - 0.5, p.y - 0.5)
        // Bottom-right corner
        glVertex2d(p.x + treeSize, p.y - treeSize); // (p.x + 0.5, p.y - 0.5)
        glEnd();

        // --- Draw the top, narrower triangle (overlapping) ---
        glBegin(GL_POLYGON);
        // Top-most point
        glVertex2d(p.x, p.y + treeSize); // (p.x, p.y + 0.5)
        // Bottom-left point (narrower base, above the absolute bottom)
        glVertex2d(p.x - (treeSize * 0.6), p.y - (treeSize * 0.2)); // (p.x - 0.3, p.y - 0.1)
        // Bottom-right point (narrower base)
        glVertex2d(p.x + (treeSize * 0.6), p.y - (treeSize * 0.2)); // (p.x + 0.3, p.y - 0.1)
        glEnd();

        glColor3d(0, 0, 0);
        glLineWidth(1.0);

        // Border for bottom triangle
        glBegin(GL_LINE_LOOP);
        glVertex2d(p.x, p.y + (treeSize * 0.4));
        glVertex2d(p.x - treeSize, p.y - treeSize);
        glVertex2d(p.x + treeSize, p.y - treeSize);
        glEnd();

        // Border for top triangle
        glBegin(GL_LINE_LOOP);
        glVertex2d(p.x, p.y + treeSize);
        glVertex2d(p.x - (treeSize * 0.6), p.y - (treeSize * 0.2));
        glVertex2d(p.x + (treeSize * 0.6), p.y - (treeSize * 0.2));
        glEnd();
    }
}

void Map::DrawRocks()
{
    double rockSize = 0.5;

    for (const auto& p : rocks)
    {
        // rock gray body
        glColor3d(0.5, 0.5, 0.5);
        glBegin(GL_POLYGON);
        glVertex2d(p.x - rockSize, p.y - rockSize);
        glVertex2d(p.x - rockSize, p.y + rockSize);
        glVertex2d(p.x + rockSize, p.y + rockSize);
        glVertex2d(p.x + rockSize, p.y - rockSize);
        glEnd();

        // Draw black border
        glColor3d(0, 0, 0);
        glLineWidth(1.0);
        glBegin(GL_LINE_LOOP);
        glVertex2d(p.x - rockSize, p.y - rockSize);
        glVertex2d(p.x - rockSize, p.y + rockSize);
        glVertex2d(p.x + rockSize, p.y + rockSize);
        glVertex2d(p.x + rockSize, p.y - rockSize);
        glEnd();
    }
}

void Map::DrawWater()
{
    double waterSize = 0.5;

    for (const auto& p : waterBlocks)
    {
        // water blue body
        glBegin(GL_POLYGON);
        glColor3d(0.6, 0.8, 1.0);
        glVertex2d(p.x - waterSize, p.y - waterSize);
        glVertex2d(p.x - waterSize, p.y + waterSize);
        glVertex2d(p.x + waterSize, p.y + waterSize);
        glVertex2d(p.x + waterSize, p.y - waterSize);
        glEnd();

        // black frame
        glColor3d(0, 0, 0);
        glLineWidth(1.0);
        glBegin(GL_LINE_LOOP);
        glVertex2d(p.x - waterSize, p.y - waterSize);
        glVertex2d(p.x - waterSize, p.y + waterSize);
        glVertex2d(p.x + waterSize, p.y + waterSize);
        glVertex2d(p.x + waterSize, p.y - waterSize);
        glEnd();

        // Add a wave symbol in the center
        glColor3d(0.0, 0.0, 0.5);
        glLineWidth(1.0);

        // Center wave
        glBegin(GL_LINE_STRIP);
        glVertex2d(p.x - 0.3, p.y);
        glVertex2d(p.x - 0.15, p.y + 0.1);
        glVertex2d(p.x, p.y);
        glVertex2d(p.x + 0.15, p.y + 0.1);
        glVertex2d(p.x + 0.3, p.y);
        glEnd();

        // Wave above the center
        glBegin(GL_LINE_STRIP);
        glVertex2d(p.x - 0.3, p.y + 0.2); // Shifted up by 0.2 units
        glVertex2d(p.x - 0.15, p.y + 0.3);
        glVertex2d(p.x, p.y + 0.2);
        glVertex2d(p.x + 0.15, p.y + 0.3);
        glVertex2d(p.x + 0.3, p.y + 0.2);
        glEnd();

        // Wave below the center
        glBegin(GL_LINE_STRIP);
        glVertex2d(p.x - 0.3, p.y - 0.2); // Shifted down by 0.2 units
        glVertex2d(p.x - 0.15, p.y - 0.1);
        glVertex2d(p.x, p.y - 0.2);
        glVertex2d(p.x + 0.15, p.y - 0.1);
        glVertex2d(p.x + 0.3, p.y - 0.2);
        glEnd();
    }
}

void Map::DrawWarehouses()
{
    double warehouse_size = 0.5;

    for (const auto& p : warehouses)
    {
        // draw body
        glBegin(GL_POLYGON);
        glColor3d(1.0, 1.0, 0.0);
        glVertex2d(p.x - warehouse_size, p.y - warehouse_size);
        glVertex2d(p.x - warehouse_size, p.y + warehouse_size);
        glVertex2d(p.x + warehouse_size, p.y + warehouse_size);
        glVertex2d(p.x + warehouse_size, p.y - warehouse_size);
        glEnd();

        // black frame
        glColor3d(0, 0, 0);
        glLineWidth(1.0);
        glBegin(GL_LINE_LOOP);
        glVertex2d(p.x - warehouse_size, p.y - warehouse_size);
        glVertex2d(p.x - warehouse_size, p.y + warehouse_size);
        glVertex2d(p.x + warehouse_size, p.y + warehouse_size);
        glVertex2d(p.x + warehouse_size, p.y - warehouse_size);
        glEnd();
    }
}

void Map::DrawField()
{
    DrawForest();
    DrawRocks();
    DrawWater();
    DrawWarehouses();
}