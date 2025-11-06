#pragma once
#include "NPC.h"

class CommanderState;

class Commander : public NPC
{
private:
	double combinedViewMap[MSX][MSY];
		
	// fsm managment
	CommanderState* currentState;
	int plannedCommand;      // Command decided by PlanningState
	Point plannedTarget;     // Target decided by PlanningState


protected:
	void CalculatePathAndMove() override;
	void UpdateCombinedViewMap();
	
	Point FindSafePosition() const;
	Point FindAttackPosition() const;

public:
	friend class CommanderAnalyzingState;
	friend class CommanderPlanningState;
	friend class CommanderIssuingOrdersState;
	friend class CommanderRepositioningState;

	// constructor
	Commander(int x, int y, TeamColor t);
	~Commander();

	std::vector<NPC*> teamMembers;
	std::vector<NPC*> injuredSoldiers;
	std::vector<NPC*> resupplySoldiers;
	std::vector<Sighting> allSpottedEnemies;

	void DoSomeWork(const double* pMap) override;

	// info from soldeiers
	void ReportSighting(NpcType enemyType, Point enemyLoc) override;
	void ReportLowAmmo(NPC* Warrior) override;
	void ReportInjury(NPC* injuredSoldier) override;

	void ExecuteCommand(int commandeCode, Point target) override {}
	
	void AddMemeber(NPC* member) {teamMembers.push_back(member);}
	void IssueCommand(int commandCode, Point target);

	// fsm managment
	void SetState(CommanderState* newState);
	CommanderState* GetState() const { return currentState; }

	// uery methods for states
	bool HasSpottedEnemies() const { return !allSpottedEnemies.empty(); }
	bool HasInjuredSoldiers() const { return !injuredSoldiers.empty(); }
	bool HasLowAmmoSoldiers() const { return !resupplySoldiers.empty(); }

	void SetPlannedCommand(int cmd, Point target)
	{
		plannedCommand = cmd;
		plannedTarget = target;
	}

	// getters
	NPC* GetNextInjuredSoldier();
	NPC* GetNextLowAmmoSoldier();
	std::vector<NPC*>& GetTeamMembers() { return teamMembers; }

	std::vector<NPC*>& GetInjuredSoldiers() { return injuredSoldiers; }
	std::vector<NPC*>& GetResupplySoldiers() { return resupplySoldiers; }

	int GetTeamMemberCount() const { return teamMembers.size(); }
	int GetWoundedCount() const;
	
	int GetPlannedCommand() const { return plannedCommand; }
	Point GetPlannedTarget() const { return plannedTarget; }

};
