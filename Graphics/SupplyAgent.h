#pragma once
#include "NPC.h"
#include "Commander.h"
#include "IPathfinding.h"
#include "Warrior.h"

class SupplyAgentState;

class SupplyAgent : public NPC, public IPathfinding
{
private:
	SupplyAgentState* currentState; // fsm state
	Point targetLocation;  // Location of warrior who needs ammo
	Point targetWarehouse[2];
	bool hasAmmo;          // Whether agent is carrying ammo
	NPC* deliveryTarget;

	int cargoAmmo = 0;
	int cargoGrenades = 0;

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
	bool IsIdle() const;
	void AssignSupplyMission(NPC* warrior);

	// Getters
	NPC* GetDeliveryTarget() const { return deliveryTarget; }
	Point GetTargetLocation() const { return targetLocation; }
	bool HasAmmo() const { return hasAmmo; }
	bool IsMoving() const { return isMoving; } // for pathfinding

	// Friend declarations for states
	friend class SupplyIdleState;
	friend class SupplyGoToWarehouseState;
	friend class SupplyGoToWarriorState;
	friend class SupplyDeliveringState;
	friend class SupplyWaitState;
};


