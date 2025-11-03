#include "CommanderPlanningState.h"
#include "CommanderIssuingOrdersState.h"
#include "Commander.h"
#include <iostream>

void CommanderPlanningState::OnEnter(Commander* commander)
{
    std::cout << "Commander (Team " << (commander->GetTeam() == TEAM_RED ? "RED" : "BLUE")
        << ") entering PLANNING state\n";
}

void CommanderPlanningState::Execute(Commander* commander)
{
    // 1. Check if enemies are spotted
    if (commander->HasSpottedEnemies())
    {
        // 2. Count how many team members can fight
        int totalMembers = commander->GetTeamMemberCount();
        int woundedCount = commander->GetWoundedCount();

        // 3. Decide strategy based on team condition
        if (woundedCount >= totalMembers / 2)
        {
            // Most team is wounded - DEFEND strategy
            std::cout << "Commander planning: DEFEND (too many wounded: "
                << woundedCount << "/" << totalMembers << ")\n";

            Point safePos = commander->FindSafePosition();
            commander->SetPlannedCommand(CMD_DEFEND, safePos);
        }
        else
        {
            // Team can fight - ATTACK strategy
            std::cout << "Commander planning: ATTACK (team ready: "
                << (totalMembers - woundedCount) << "/" << totalMembers << ")\n";

            Point attackPos = commander->FindAttackPosition();
            commander->SetPlannedCommand(CMD_ATTACK, attackPos);
        }

        // 4. Transition to issuing orders
        commander->SetState(new CommanderIssuingOrdersState());
    }
    else
    {
        // No enemies spotted - move to center or patrol
        std::cout << "Commander planning: MOVE (no enemies spotted)\n";

        Point centerPos = { MSX / 2, MSY / 2 };
        commander->SetPlannedCommand(CMD_MOVE, centerPos);

        commander->SetState(new CommanderIssuingOrdersState());
    }
}

void CommanderPlanningState::OnExit(Commander* commander)
{
    std::cout << "Commander exiting PLANNING state\n";
}