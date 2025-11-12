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

        // 1. קביעת אזור המרכז לחקירה (25% עד 75% מהמפה)
        int minX = (int)(MSX * 0.25); // 20
        int maxX = (int)(MSX * 0.75); // 60
        int minY = (int)(MSY * 0.25); // 12
        int maxY = (int)(MSY * 0.75); // 36

        // 2. המטרה: נקודה אקראית בטווח המרכזי
        // זה מבטיח שהמפקד יעזוב את הפינות ויתכנס למרכז המפה.
        strategicPoint.x = minX + (rand() % (maxX - minX));
        strategicPoint.y = minY + (rand() % (maxY - minY));

        std::cout << "Commander moving to strategic patrol point: ("
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