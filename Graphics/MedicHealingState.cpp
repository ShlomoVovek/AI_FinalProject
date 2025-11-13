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
    // 1. בדיקת ריפוי עצמי (אם בחרתם להפריד את הלוגיקה מהתור)
    if (medic->NeedsSelfHeal())
    {
        medic->heal(HEAL_AMOUNT_PER_TICK);
        if (!medic->NeedsSelfHeal())
        {
            std::cout << "Medic self-heal complete. Checking mission queue.\\n";
            if (medic->GetNextPatient() == nullptr)
                medic->SetState(new MedicIdleState());
            else
                medic->SetState(new MedicGoToTargetState());
        }
        return; // סיום, כי ריפוי עצמי קודם
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
        // מרפא. הפונקציה patient->heal(amount) מבטיחה שלא נעבור את MAX_HP.
        patient->heal(HEAL_AMOUNT_PER_TICK);

        if (patient->GetHealth() >= MAX_HP)
        {
            std::cout << "Patient fully healed. Checking next patient.\\n";
            medic->RemoveCurrentPatient(); // ** הסר פצוע שהגיע ל-MAX_HP **

            if (medic->GetNextPatient() == nullptr)
            {
                std::cout << "No more patients. Medic going idle.\\n";
                medic->SetState(new MedicIdleState());
            }
            else
            {
                // פונה לפצוע הבא
                std::cout << "Moving to next patient in queue.\\n";
                medic->SetState(new MedicGoToTargetState());
            }
        }
    }
    else
    {
        // הפצוע התרחק תוך כדי ריפוי
        std::cout << "Patient moved away while healing. Returning to GO_TO_TARGET.\\n";
        medic->SetState(new MedicGoToTargetState());
    }
}

void MedicHealingState::OnExit(Medic* agent)
{
    std::cout << "Medic exiting HEALING state.\n";
}