#include "CommanderAnalyzingState.h"
#include "CommanderPlanningState.h"
#include "CommanderRepositioningState.h"
#include "CommanderIssuingOrdersState.h"
#include "Commander.h"
#include <iostream>

void CommanderAnalyzingState::OnEnter(Commander* commander)
{
    std::cout << "Commander (Team " << (commander->GetTeam() == TEAM_RED ? "RED" : "BLUE")
        << ") entering ANALYZING state\n";
}

void CommanderAnalyzingState::Execute(Commander* commander)
{
    // 1. CRITICAL HEALTH: Stop everything and retreat
    if (commander->GetHealth() < 30)
    {
        std::cout << "Commander CRITICAL HEALTH: " << commander->GetHealth()
            << " - EMERGENCY RETREAT!\n";
        commander->ReportInjury(commander);  // Request medic
        commander->SetState(new CommanderRepositioningState(true));
        return;
    }

    if (commander->HasCriticalInjuredSoldiers())
    {
        std::cout << "Commander (Analyzing): Critical injury reported! Issuing RETREAT command to all units.\n";

        commander->SetPlannedCommand(CMD_RETREAT, commander->GetLocation());

        // מעבר מיידי למצב IssuingOrders כדי לשלוח את הפקודה
        commander->SetState(new CommanderIssuingOrdersState());
        return;
    }

    // 2. Low health: Request medic and consider retreating
    if (commander->GetHealth() < 60)
    {
        std::cout << "Commander requesting medic! Health: "
            << commander->GetHealth() << "\n";
        commander->ReportInjury(commander);
    }

    // 3. Update combined visibility map
    commander->UpdateCombinedViewMap();

    // 4. Handle support missions
    HandleMedicRequests(commander);
    HandleSupplyRequests(commander);

    // 5. Decide next action
    if (commander->HasSpottedEnemies())
    {
        if (commander->GetHealth() < 50)
        {
            std::cout << "Commander: Enemies spotted but health low ("
                << commander->GetHealth() << "). Retreating!\n";
            commander->SetState(new CommanderRepositioningState(true));
        }
        else
        {
            std::cout << "Commander (Analyzing): Enemy spotted! Moving to PLANNING.\n";
            commander->SetState(new CommanderPlanningState());
        }
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
    // 1. נבדוק אם יש לוחמים שמחכים לאספקה
    if (commander->HasLowAmmoSoldiers())
    {
        // 2. נמצא את ה-SupplyAgent
        SupplyAgent* agent = nullptr;
        for (NPC* member : commander->GetTeamMembers())
        {
            if (member->GetType() == SUPPLIER && member->IsAlive())
            {
                agent = static_cast<SupplyAgent*>(member);
                break; // מצאנו סוכן אספקה אחד
            }
        }

        if (agent != nullptr)
        {
            std::cout << "Commander (Analyzing): Assigning supply mission(s) to agent.\n";

            // 3. **שינוי עיקרי:** נעביר את כל הלוחמים הממתינים לתור של ה-SupplyAgent
            // נשתמש בלולאה כדי להוציא את כל הלוחמים מהתור של המפקד
            while (commander->HasLowAmmoSoldiers())
            {
                // שימוש בפונקציה קיימת שמחזירה את הראשון ומסירה אותו
                NPC* lowAmmoWarrior = commander->GetNextLowAmmoSoldier();

                if (lowAmmoWarrior != nullptr)
                {
                    // נשלח את הלוחם ל-SupplyAgent, שינהל אותו בתור הפנימי שלו
                    agent->AssignSupplyMission(lowAmmoWarrior);
                }
            }
        }
        else
        {
            std::cout << "Commander (Analyzing): Supply Agent not found or is dead.\n";
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



