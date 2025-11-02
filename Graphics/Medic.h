#pragma once
#include "NPC.h"
#include "IPathfinding.h"

enum MedicState { M_IDLE, M_HEALING, M_MOVING_TO_BASE, M_MOVING_TO_TARGET, M_RETREAT };

class Medic : public NPC, public IPathfinding
{
private:
	// medic status
	
	MedicState currentState;
	NPC* patientTarget; // will use FindAStarPath() for path from base to patient
	Point finalTargetLocation; // patientTarget->GetLocation()

protected:
	void CalculatePathAndMove() override;
	Point GetBaseLocation() const;
	Point GetLoaction() const override { return NPC::GetLocation(); }
public: 
	// constructor
	Medic(int x, int y, TeamColor t);

	void DoSomeWork(const double* pMap) override;

	// NPC functions
	void ReportSighting(NpcType enemyType, Point enemyLoc) override;
	void ReportLowAmmo(NPC* warrior) override;
	void ReportInjury(NPC* injuredSoldier) override;
	void ExecuteCommand(int commandCode, Point target) override;
};







