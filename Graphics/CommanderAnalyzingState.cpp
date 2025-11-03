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

    // 2. Process urgent reports first (injury/ammo)
    if (commander->HasInjuredSoldiers())
    {
        NPC* injured = commander->GetNextInjuredSoldier();
        if (injured)
        {
            commander->IssueCommand(CMD_HEAL, injured->GetLocation());
            std::cout << "Commander issued HEAL command for injured soldier\n";
        }
        return; // Handle one urgent issue per frame
    }

    if (commander->HasLowAmmoSoldiers())
    {
        NPC* needsAmmo = commander->GetNextLowAmmoSoldier();
        if (needsAmmo)
        {
            commander->IssueCommand(CMD_RESUPPLY, needsAmmo->GetLocation());
            std::cout << "Commander issued RESUPPLY command\n";
        }
        return; // Handle one urgent issue per frame
    }

    // 3. If no urgent issues, transition to planning
    commander->SetState(new CommanderPlanningState());
}

void CommanderAnalyzingState::OnExit(Commander* commander)
{
    std::cout << "Commander exiting ANALYZING state\n";
}