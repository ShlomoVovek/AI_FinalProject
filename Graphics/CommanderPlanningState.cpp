#include "CommanderPlanningState.h"
#include "CommanderIssuingOrdersState.h"
#include "CommanderRepositioningState.h"
#include "CommanderAnalyzingState.h"
#include "Commander.h"
#include <iostream>

void CommanderPlanningState::OnEnter(Commander* commander)
{
    std::cout << "Commander (Team " << (commander->GetTeam() == TEAM_RED ? "RED" : "BLUE")
        << ") entering PLANNING state\n";
}

void CommanderPlanningState::Execute(Commander* commander)
{
    if (commander->GetHealth() < 40)
    {
        std::cout << "Commander (Planning): Critical health! Aborting plan and retreating!\n";
        commander->SetState(new CommanderRepositioningState());
        return;
    }

    // COOLDOWN
    if ((commander->GetLastMajorCommand() == CMD_ATTACK || commander->GetLastMajorCommand() == CMD_DEFEND) &&
        commander->GetFramesSinceLastMajorCommand() < commander->COMMAND_COOLDOWN)
    {
        std::cout << "Commander planning: STICKING to last major command (ATTACK/DEFEND) until cooldown expires.\n";
        // commander->SetState(new CommanderRepositioningState());
        return;
    }

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

        commander->SetLastMajorCommand(commander->GetPlannedCommand());
        commander->ResetFramesSinceLastMajorCommand();

        // 4. Transition to issuing orders
        commander->SetState(new CommanderIssuingOrdersState());
    }
    else
    {
        std::cout << "Commander planning: MOVE (no enemies spotted)\n";

        Point currentPos = commander->GetLocation();
        Point strategicPoint;

        int minX = (int)(MSX * 0.25);
        int maxX = (int)(MSX * 0.75);
        int minY = (int)(MSY * 0.25);
        int maxY = (int)(MSY * 0.75);

        strategicPoint.x = minX + (rand() % (maxX - minX));
        strategicPoint.y = minY + (rand() % (maxY - minY));

        std::cout << "Commander moving to strategic patrol point: ("
            << strategicPoint.x << ", " << strategicPoint.y << ")\n";

        commander->SetPlannedCommand(CMD_MOVE, strategicPoint);

        commander->SetLastMajorCommand(CMD_MOVE);
        commander->ResetFramesSinceLastMajorCommand();

        commander->SetState(new CommanderIssuingOrdersState());
    }
}

void CommanderPlanningState::OnExit(Commander* commander)
{
    std::cout << "Commander exiting PLANNING state\n";
}

bool CommanderPlanningState::AreTeamMembersAtTarget(Commander* commander, Point target)
{
    int membersAtTarget = 0;
    int totalMembers = 0;

    for (NPC* member : commander->GetTeamMembers())
    {
        if (member->IsAlive() && member->GetType() != COMMANDER)
        {
            totalMembers++;
            Point memberLoc = member->GetLocation();

            // Check if member is within 3 units of target
            double dist = std::sqrt(
                (memberLoc.x - target.x) * (memberLoc.x - target.x) +
                (memberLoc.y - target.y) * (memberLoc.y - target.y)
            );

            if (dist <= 3.0)
            {
                membersAtTarget++;
            }
        }
    }

    // If most team members are already at target, return true
    return (totalMembers > 0 && membersAtTarget >= totalMembers / 2);
}