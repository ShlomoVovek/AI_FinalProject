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
        // No enemies spotted - advance strategically
        std::cout << "Commander planning: MOVE (no enemies spotted)\n";

        Point currentPos = commander->GetLocation();
        Point strategicPoint;

        // Determine which side of the map we're on
        bool isLeftSide = (commander->GetTeam() == TEAM_RED);

        // If on left side, move toward right (enemy territory)
        // If on right side, move toward left (enemy territory)
        if (isLeftSide)
        {
            // Red team moves from left toward center-right
            strategicPoint.x = MSX * 0.6 + (rand() % 10) - 5; // 60% across + random
        }
        else
        {
            // Blue team moves from right toward center-left
            strategicPoint.x = MSX * 0.4 + (rand() % 10) - 5; // 40% across + random
        }

        // Keep similar Y coordinate but add some variation
        strategicPoint.y = currentPos.y + (rand() % 10) - 5;

        // Clamp to map boundaries
        if (strategicPoint.x < 5) strategicPoint.x = 5;
        if (strategicPoint.x >= MSX - 5) strategicPoint.x = MSX - 6;
        if (strategicPoint.y < 5) strategicPoint.y = 5;
        if (strategicPoint.y >= MSY - 5) strategicPoint.y = MSY - 6;

        std::cout << "Commander moving forward to patrol point: ("
            << strategicPoint.x << ", " << strategicPoint.y << ")\n";

        commander->SetPlannedCommand(CMD_MOVE, strategicPoint);
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