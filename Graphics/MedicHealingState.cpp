#include "MedicHealingState.h"
#include "Medic.h"
#include "MedicIdleState.h" // Back to idle when done
#include <iostream>

void MedicHealingState::OnEnter(Medic* medic)
{
    std::cout << "Medic entering Healing state.\n";
    medic->isMoving = false;
    medic->currentPath.clear();
}

void MedicHealingState::Execute(Medic* medic)
{
    if (medic->NeedsSelfHeal())
    {
        if (medic->HasMedicine())
        {
            medic->heal(HEAL_AMOUNT_PER_TICK);
            medic->UseMedicine(HEAL_AMOUNT_PER_TICK);

            if (!medic->NeedsSelfHeal())
            {
                std::cout << "Medic self-heal complete. Checking mission queue.\\n";
                if (medic->GetNextPatient() == nullptr)
                    medic->SetState(new MedicIdleState());
                else
                    medic->SetState(new MedicGoToTargetState());
            }
        }
        else
        {
            std::cout << "Medic needs self-heal but has no medicine. Going to base.\\n";
            medic->SetState(new MedicGoToBaseState());
        }
        return;
    }

    // 2. לוגיקת ריפוי פצועים חיצוניים (מהתור)
    NPC* patient = medic->GetNextPatient();

    // בדיקת תקינות: האם פצוע קיים, חי, או כבר נרפא
    if (!patient || !patient->IsAlive() || patient->GetHealth() >= MAX_HP)
    {
        if (patient)
            medic->RemoveCurrentPatient(); // הסר את הפצוע הלא תקין/רפא

        if (medic->GetNextPatient() == nullptr)
        {
            // אין יותר פצועים
            std::cout << "No more patients. Medic going idle.\\n";
            medic->SetState(new MedicIdleState());
        }
        else
        {
            // יש פצוע נוסף - עבור לטפל בו
            std::cout << "Patient gone/healed. Moving to next patient in queue.\\n";
            medic->SetState(new MedicGoToTargetState());
        }
        return;
    }

    // 3. ריפוי בפועל
    Point medicLoc = medic->GetLocation();
    Point patientLoc = patient->GetLocation();
    double dist = Distance(medicLoc, patientLoc);

    if (dist <= 2.0)
    {
        if (medic->HasMedicine())
        {
            patient->heal(HEAL_AMOUNT_PER_TICK);
            medic->UseMedicine(HEAL_AMOUNT_PER_TICK);

            if (patient->GetHealth() >= MAX_HP)
            {
                std::cout << "Patient fully healed. Checking next patient.\\n";
                medic->RemoveCurrentPatient();

                if (medic->GetNextPatient() == nullptr)
                {
                    std::cout << "No more patients. Medic going to base (RETREAT).\\n";
                    medic->SetState(new MedicGoToBaseState());
                }
                else
                {
                    std::cout << "Moving to next patient in queue.\\n";
                    medic->SetState(new MedicGoToTargetState());
                }
            }
        }
        else
        {
            std::cout << "Medic ran out of medicine. Going back to base.\\n";
            medic->SetState(new MedicGoToBaseState());
        }
    }
    else
    {
        std::cout << "Patient moved away while healing. Returning to GO_TO_TARGET.\\n";
        medic->SetState(new MedicGoToTargetState());
    }
}

void MedicHealingState::OnExit(Medic* agent)
{
    std::cout << "Medic exiting HEALING state.\n";
}