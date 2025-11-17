#pragma once
#include "NPC.h"
#include "Commander.h"
#include "IPathfinding.h"
#include "MedicState.h"
#include "MedicIdleState.h"
#include "MedicGoToBaseState.h"
#include "MedicGoToTargetState.h"
#include "MedicHealingState.h"
#include <list>

class Medic : public NPC, public IPathfinding
{
private:
	// medic status
	
	MedicState* currentState;
	std::list<NPC*> patientsQueue; // will use FindAStarPath() for path from base to patient
	Point finalTargetLocation; 

	double currentMedicineSupply;

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

	const double MAX_MEDICINE_SUPPLY = 100;
	const double SELF_HEAL_THRESHOLD = 0.7 * MAX_HP;
	void RestockMedicine() { currentMedicineSupply = MAX_MEDICINE_SUPPLY; }
	void UseMedicine(double amount) { currentMedicineSupply -= amount; if (currentMedicineSupply < 0) currentMedicineSupply = 0; }
	bool HasMedicine() const { return currentMedicineSupply > 0; }
	double GetCurrentSupply() const { return currentMedicineSupply; }


	// fsm Management
	void SetState(MedicState* newState);
	MedicState* GetState() const { return currentState; }

	void DoSomeWork(const double* pMap) override;

	Point GetFinalTargetLocation() const { return finalTargetLocation; }
	NPC* GetPatientTarget() const { return patientsQueue.empty() ? nullptr : patientsQueue.front(); }
	bool IsMoving() const { return isMoving; } // for pathfinding
	bool IsIdle() const;
	void AssignHealMission(NPC* injuredSoldier);

	NPC* GetNextPatient();
	void RemoveCurrentPatient();

	// NPC functions
	void ReportSighting(NpcType enemyType, Point enemyLoc) override;
	void ReportLowAmmo(NPC* warrior) override;
	void ReportInjury(NPC* injuredSoldier) override;
	void ExecuteCommand(int commandCode, Point target) override;

	// healing methods
	bool NeedsSelfHeal() const;
};







