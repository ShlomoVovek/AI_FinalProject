#include "CommanderAnalyzingState.h"
#include "CommanderPlanningState.h"
#include "Commander.h"
#include <iostream>

void CommanderAnalyzingState::OnEnter(Commander* commander)
{
    std::cout << "Commander (Team " << (commander->GetTeam() == TEAM_RED ? "RED" : "BLUE")
        << ") entering ANALYZING state\n";
}

void CommanderAnalyzingState::Execute(Commander* commander)
{
    // 1. Update combined visibility map from all team members
    commander->UpdateCombinedViewMap();

    HandleMedicRequests(commander);
    HandleSupplyRequests(commander);

    // 3. בדוק תנאים אסטרטגיים למעבר למצב תכנון
    bool enemySpotted = commander->HasSpottedEnemies(); 

    if (enemySpotted) {
        std::cout << "Commander (Analyzing): Enemy spotted! Moving to PLANNING.\n";
    }
    else {
        std::cout << "Commander (Analyzing): No immediate threats. Moving to PLANNING for next move.\n";
    }

    // Always transition to Planning state
    commander->SetState(new CommanderPlanningState());
}

void CommanderAnalyzingState::OnExit(Commander* commander)
{
    std::cout << "Commander exiting ANALYZING state\n";
}

void CommanderAnalyzingState::HandleSupplyRequests(Commander* commander)
{
    // קח את רשימת הבקשות מהמפקד
    std::vector<NPC*>& requests = commander->GetResupplySoldiers();
    if (requests.empty()) {
        return; // אין בקשות, צא
    }

    NPC* needyWarriorNPC = requests.front();

    // 1. ודא שהלוחם עדיין זקוק לאספקה
    Warrior* needyWarrior = static_cast<Warrior*>(needyWarriorNPC);
    if (needyWarrior == nullptr || !needyWarrior->IsAlive() || !needyWarrior->HasRequestedSupply())
    {
        requests.erase(requests.begin());
        return;
    }

    // 2. חפש סוכן אספקה פנוי
    for (NPC* agent : commander->GetTeamMembers())
    {
        if (agent->GetTeam() == commander->GetTeam() && agent->GetType() == SUPPLIER)
        {
            SupplyAgent* supplyAgent = static_cast<SupplyAgent*>(agent);

            // 3. אם מצאת סוכן פנוי, הקצה לו את המשימה
            if (supplyAgent && supplyAgent->IsIdle())
            {
                std::cout << "Commander (Analyzing): Assigning supply mission to agent.\n";
                supplyAgent->AssignSupplyMission(needyWarrior);

                requests.erase(requests.begin());
                return; 
            }
        }
    }
    // אם לא נמצא סוכן פנוי, הבקשה תישאר בתור לפריים הבא
}

void CommanderAnalyzingState::HandleMedicRequests(Commander* commander)
{
    std::vector<NPC*>& requests = commander->GetInjuredSoldiers();
    if (requests.empty()) {
        return;
    }

    NPC* injuredSoldier = requests.front();

    // 1. ודא שהחייל עדיין פצוע וחי
    if (injuredSoldier == nullptr || !injuredSoldier->IsAlive() || injuredSoldier->GetHealth() > CRITICAL_HP)
    {
        requests.erase(requests.begin());
        return;
    }

    // 2. חפש מדיק פנוי
    for (NPC* agent : commander->GetTeamMembers())
    {
        if (agent->GetTeam() == commander->GetTeam() && agent->GetType() == MEDIC)
        {
            Medic* medic = static_cast<Medic*>(agent);

            if (medic && medic->IsIdle())
            {
                std::cout << "Commander (Analyzing): Assigning heal mission to medic.\n";
                medic->AssignHealMission(injuredSoldier);

                requests.erase(requests.begin());
                return;
            }
        }
    }
}



