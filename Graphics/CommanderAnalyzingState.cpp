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
    bool enemiesSpotted = false;
    // Get team members to check their recent sightings
    for (NPC* member : commander->GetTeamMembers())
    {
        if (member->IsAlive() && member->GetType() == WARRIOR)
        {
            Warrior* warrior = static_cast<Warrior*>(member);

            // Check if warrior sees enemies (this should be done via view map)
            // For now, we'll rely on the HasSpottedEnemies flag being set by warriors
        }
    }

    // Handle support missions
    HandleMedicRequests(commander);
    HandleSupplyRequests(commander);

    // Decide next action based on situation
    if (commander->HasSpottedEnemies())
    {
        std::cout << "Commander (Analyzing): Enemy spotted! Moving to PLANNING.\n";
        commander->SetState(new CommanderPlanningState());
    }
    else
    {
        std::cout << "Commander (Analyzing): No immediate threats. Moving to PLANNING for next move.\n";
        commander->SetState(new CommanderPlanningState());
    }
}

void CommanderAnalyzingState::OnExit(Commander* commander)
{
    std::cout << "Commander exiting ANALYZING state\n";
}

void CommanderAnalyzingState::HandleSupplyRequests(Commander* commander)
{
    if (commander->HasLowAmmoSoldiers())
    {
        std::cout << "Commander (Analyzing): Assigning supply mission to agent.\n";

        for (NPC* member : commander->GetTeamMembers())
        {
            if (member->GetType() == SUPPLIER && member->IsAlive())
            {
                SupplyAgent* agent = static_cast<SupplyAgent*>(member);
                NPC* lowAmmo = commander->GetNextLowAmmoSoldier();

                if (lowAmmo != nullptr)
                {
                    agent->AssignSupplyMission(lowAmmo);
                }
            }
        }
    }
}

void CommanderAnalyzingState::HandleMedicRequests(Commander* commander)
{
    if (commander->HasInjuredSoldiers())
    {
        std::cout << "Commander (Analyzing): Assigning heal mission to medic.\n";

        for (NPC* member : commander->GetTeamMembers())
        {
            if (member->GetType() == MEDIC && member->IsAlive())
            {
                Medic* medic = static_cast<Medic*>(member);
                NPC* injured = commander->GetNextInjuredSoldier();

                if (injured != nullptr)
                {
                    medic->AssignHealMission(injured);
                }
            }
        }
    }
}



