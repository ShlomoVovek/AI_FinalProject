# AI_FinalProject: Autonomous Battle Simulation

Shlomo Vovek - Shlomi.vov@gmail.com 
Shaked Chen - Shaked.Chen2@s.afeka.ac.il ID:315535625
github: https://github.com/ShlomoVovek/AI_FinalProject/tree/main/Graphics


1. Instructions:wWhile the program is running, press 'R' to restart or 'Esc' to exit.

2. KNOWN ISSUES & BUGS:
     We added 'Survial Mode' for cases where a Commander is dead but its soldiers are still alive. 
     It gives the warrior the option to keep search for the enemy, and
     However, here are cases where the game will not end due to the strategic conditions such as:
     * Both sides' Supply Agent are dead, the ammo and grenades are done and the other NPCs are still alive
     * Both sides' Commander is dead and the warrior are not efficient enough to kill their rivals before they regenerate HP.
     * Cases where the game run into other dead loops.
     
     In cases like these the game will not end and but keep run until the user will press 'R' or 'Esc'.

     In some cases when the Medic is fleeing, it is look like he is exiting the map in the corners of the map

     It is possible to two NPCs to enter the same cell in the map. Mostly happens with Warriors.

3.  **Prerequisites:** You must have Microsoft Visual Studio 2022 (or compatible) installed. The **freeglut** and **glew32** libraries must be configured and linked. C++ (Built with **Visual Studio v143 Platform Toolset**).