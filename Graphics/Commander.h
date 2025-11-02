#pragma once
#include "NPC.h"

class Commander : public NPC
{
private:
	double combinedViewMap[MSX][MSY];
	int currentTeamCommand;
	Point currentTeamTarget;

	std::vector<NPC*> teamMembers;	
	std::vector<NPC*> injuredSoldiers;
	std::vector<NPC*> resupplySoldires;
	std::vector<Sighting> allSpottedEnemies;


protected:
	void CalculatePathAndMove() override;
	void UpdateCombinedViewMap();
	
	Point FindSafePosition() const;
	Point FindAttackPosition() const;

public:
	

	// constructor
	Commander(int x, int y, TeamColor t);
	void DoSomeWork(const double* pMap) override;

	// info from soldeiers
	void ReportSighting(NpcType enemyType, Point enemyLoc) override;
	void ReportLowAmmo(NPC* Warrior) override;
	void ReportInjury(NPC* injuredSoldier) override;

	void ExecuteCommand(int commandeCode, Point target) override {}
	
	void AddMemeber(NPC* member) {teamMembers.push_back(member);}
	void IssueCommand(int commandCode, Point target);
};
