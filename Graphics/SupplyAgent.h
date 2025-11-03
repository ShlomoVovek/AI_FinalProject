#pragma once
#include "NPC.h"
#include "IPathfinding.h"

class SupplyAgentState;

class SupplyAgent : public NPC, public IPathfinding
{
private:
	SupplyAgentState* currentState;
	Point targetLocation;  // Location of warrior who needs ammo
	bool hasAmmo;          // Whether agent is carrying ammo

protected: 
	void CalculatePathAndMove() override;
	Point GetBaseLocation() const;
	Point GetLocation() const override { return NPC::GetLocation(); }
public: 
	// constructor
	SupplyAgent(int x, int y, TeamColor t);
	~SupplyAgent();

	void DoSomeWork(const double* pMap) override;

	// FSM Management
	void SetState(SupplyAgentState* newState);
	SupplyAgentState* GetState() const { return currentState; }

	// NPC methods
	void ReportSighting(NpcType enemyType, Point enemyLoc) override;
	void ReportLowAmmo(NPC* warrior) override;
	void ReportInjury(NPC* injuredSoldier) override;
	void ExecuteCommand(int commandCode, Point target) override;

	// Supply-specific methods
	Point FindNearestWarehouse() const;
	void PickupAmmo();
	void DeliverAmmo();

	// Getters
	Point GetTargetLocation() const { return targetLocation; }
	bool HasAmmo() const { return hasAmmo; }

	// For pathfinding
	bool IsMoving() const { return isMoving; }

	// Friend declarations for states
	friend class SupplyIdleState;
	friend class SupplyGoToWarehouseState;
	friend class SupplyGoToWarriorState;
	friend class SupplyDeliveringState;
};




















