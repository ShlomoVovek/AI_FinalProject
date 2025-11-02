#pragma once
#include "NPC.h"
#include "IPathfinding.h"

class SupplyAgent : public NPC, public IPathfinding
{
private:
	enum SupplyState { S_IDLE, S_FETCHING_SUPPLY, S_DELIVERING, S_RETREAT };
	SupplyState currentState;

	int carriedAmmo;
	Point targetWarehouse;
	NPC* deliveryTarget;

protected: 
	void CalculatePathAndMove() override;
	Point GetBaseLocation() const;
	Point GetLoaction() const override { return NPC::GetLocation(); }
public: 
	// constructor
	SupplyAgent(int x, int y, TeamColor t);

	void DoSomeWork(const double* pMap) override;

	// NPC methods
	void ReportSighting(NpcType enemyType, Point enemyLoc) override;
	void ReportLowAmmo(NPC* warrior) override;
	void ReportInjury(NPC* injuredSoldier) override;
	void ExecuteCommand(int commandCode, Point target) override;
};




















