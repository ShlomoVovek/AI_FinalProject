#include "CommanderIssuingOrdersState.h"
#include "CommanderRepositioningState.h"
#include "Commander.h"
#include <iostream>

void CommanderIssuingOrdersState::OnEnter(Commander* commander)
{
    std::cout << "Commander (Team " << (commander->GetTeam() == TEAM_RED ? "RED" : "BLUE")
        << ") entering ISSUING_ORDERS state\n";
    ordersIssued = false;
}

void CommanderIssuingOrdersState::Execute(Commander* commander)
{
    if (!ordersIssued)
    {
        // Get the planned command from Planning state
        int command = commander->GetPlannedCommand();
        Point target = commander->GetPlannedTarget();

        // Issue command to all team members
        commander->IssueCommand(command, target);

        std::cout << "Commander issued command: " << command
            << " to target (" << target.x << ", " << target.y << ")\n";

        ordersIssued = true;
    }

    // After issuing orders, transition to repositioning
    commander->SetState(new CommanderRepositioningState());
}

void CommanderIssuingOrdersState::OnExit(Commander* commander)
{
    std::cout << "Commander exiting ISSUING_ORDERS state\n";
}