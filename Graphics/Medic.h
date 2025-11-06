#pragma once
#include "NPC.h"
#include "Commander.h"
#include "IPathfinding.h"
#include "MedicState.h"
#include "MedicIdleState.h"
#include "MedicGoToBaseState.h"
#include "MedicGoToTargetState.h"
#include "MedicHealingState.h"

class Medic : public NPC, public IPathfinding
{
private:
	// medic status
	
	MedicState* currentState;
	NPC* patientTarget; // will use FindAStarPath() for path from base to patient
	Point finalTargetLocation; // patientTarget->GetLocation()

protected:
	void CalculatePathAndMove() override;
	Point GetBaseLocation() const;
	Point GetLocation() const override { return NPC::GetLocation(); }
public: 
	friend class MedicIdleState;
	friend class MedicGoToBaseState;
	friend class MedicGoToTargetState;
	friend class MedicHealingState;

	// constructor
	Medic(int x, int y, TeamColor t);
	~Medic();

	// fsm Management
	void SetState(MedicState* newState);
	MedicState* GetState() const { return currentState; }

	void DoSomeWork(const double* pMap) override;

	Point GetFinalTargetLocation() const { return finalTargetLocation; }
	NPC* GetPatientTarget() const { return patientTarget; }
	void SetPatientTarget(NPC* patient) { patientTarget = patient; }
	bool IsMoving() const { return isMoving; } // for pathfinding
	bool IsIdle() const;
	void AssignHealMission(NPC* injuredSoldier);

	// NPC functions
	void ReportSighting(NpcType enemyType, Point enemyLoc) override;
	void ReportLowAmmo(NPC* warrior) override;
	void ReportInjury(NPC* injuredSoldier) override;
	void ExecuteCommand(int commandCode, Point target) override;

	// healing methods


};







