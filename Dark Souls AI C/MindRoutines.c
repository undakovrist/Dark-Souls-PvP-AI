#include "MindRoutines.h"
#include <stdlib.h>
#pragma warning( disable: 4244 )

#define SCALE(input, minVal, maxVal) (2 * ((float)input - minVal) / (maxVal - minVal) - 1)

DWORD WINAPI DefenseMindProcess(void* data){
    while (!defense_mind_input->exit)
    {
        //lock control of this resource
        EnterCriticalSection(&(defense_mind_input->crit));
        //wait for the indicator this should run, and release lock in meantime
        while (defense_mind_input->runNetwork == false){
            SleepConditionVariableCS(&(defense_mind_input->cond), &(defense_mind_input->crit), INFINITE);
        }

        //generate inputs and scale from -1 to 1 
        fann_type input[8];

        //copy inputs into input and scale
        float mostRecentDistance = distance(&Player, &Enemy);
        input[0] = SCALE(mostRecentDistance, 0, 10);
        input[0] = input[0] > 1 ? 1 : input[0];
        input[0] = input[0] < -1 ? -1 : input[0];
        for (int i = 0; i < 4; i++){
            input[i+1] = SCALE(DistanceMemory[i], 0, 10);
            //cut off above and below
            input[i+1] = input[i+1] > 1 ? 1 : input[i+1];
            input[i+1] = input[i+1] < -1 ? -1 : input[i+1];
        }
        input[5] = SCALE(angleDeltaFromFront(&Player, &Enemy), 0, 1.6);
        input[6] = SCALE(Enemy.velocity, -0.18, -0.04);
        input[7] = SCALE(rotationDifferenceFromSelf(&Player, &Enemy), 0, 3.8);

        fann_type* out = fann_run(defense_mind_input->mind, input);
		//printf("%f\n", *out);

		//backstab attempt detection and avoidace
		//TODO implement more types of backstab avoidance actions
        if (*out < 10 && *out > 0.5
            && mostRecentDistance < 5 //hardcode bs distance
            && Enemy.subanimation == SubanimationNeutral //enemy cant backstab when in animation
			//&& BackstabDetection(&Enemy, &Player, mostRecentDistance) == 0 //can't be backstabed when behind enemy
        ){
			//TODO make this strafe in the same direction as the enemy strafe
            DefenseChoice = CounterStrafeLeftId;
        } 

		//if we're waking up from a bs, try to avoid chain
		if (Player.in_backstab){
			int DodgeType = rand() % 4;
			switch (DodgeType) {
			case 0:
				DefenseChoice = OmnistepBackwardsId;
				break;
			case 1:
				DefenseChoice = BarrelLeftId;
				break;
			case 2:
				DefenseChoice = ReverseRollBSId;
				break;
			case 3:
				DefenseChoice = BarrelRightId;
				break;
			}

			//if (rand() > RAND_MAX / 2){
				//randomly choose between chain escapes to through off predictions
				//DefenseChoice = OmnistepBackwardsId;
			//}
			//else{
				//DefenseChoice = ReverseRollBSId;
			//}

		}

		//if the enemy is close behind us, and there's no possibilty of chain(which a bs cancel can't prevent) try to damage cancel their bs.
		if (BackstabDetection(&Enemy, &Player, mostRecentDistance) && !Player.in_backstab && !Enemy.in_backstab) {
			switch (Player.WeaponRoutines)
			{
			case 0:
				AttackChoice = GhostHitId;
				break;
			case 4:
				AttackChoice = GhostHitId;
				break;
			case 6:
				AttackChoice = GhostHitId;
				break;
			case 7:
				AttackChoice = GhostHitId;
				break;
			case 8:
				AttackChoice = GhostHitId;
				break;
			default:
				AttackChoice = KickId;
				break;
			}
		}

        //prevent rerun
        defense_mind_input->runNetwork = false;
        //release lock
        LeaveCriticalSection(&(defense_mind_input->crit));
        WakeConditionVariable(&(defense_mind_input->cond));
    }
    return 0;
}

DWORD WINAPI AttackMindProcess(void* data){
    while (!attack_mind_input->exit)
    {
        //lock control of this resource
        EnterCriticalSection(&(attack_mind_input->crit));
        //wait for the indicator this should run, and release lock in meantime
        while (attack_mind_input->runNetwork == false){
            SleepConditionVariableCS(&(attack_mind_input->cond), &(attack_mind_input->crit), INFINITE);
        }

        //generate inputs and scale from -1 to 1 
        fann_type input[DistanceMemoryLENGTH + 5 + AIHPMemoryLENGTH + 1 + last_animation_types_enemy_LENGTH + 1];

        //copy inputs into input and scale
        float mostRecentDistance = distance(&Player, &Enemy);
        input[0] = SCALE(mostRecentDistance, 0, 10);
        input[0] = input[0] > 1 ? 1 : input[0];
        input[0] = input[0] < -1 ? -1 : input[0];
        for (int i = 0; i < DistanceMemoryLENGTH-1; i++){
            input[i+1] = SCALE(DistanceMemory[i], 0, 10);
            //cut off above and below
            input[i+1] = input[i+1] > 1 ? 1 : input[i+1];
            input[i+1] = input[i+1] < -1 ? -1 : input[i+1];
        }
        input[DistanceMemoryLENGTH] = SCALE(StaminaEstimationEnemy(), -40, 192);
        input[DistanceMemoryLENGTH + 1] = SCALE(Enemy.poise, 0, 120);
        input[DistanceMemoryLENGTH + 2] = SCALE(PoiseDamageForAttack(Player.r_weapon_id, 46), 0, 80); //Currently hardcoded for CB because jankery reasons
        input[DistanceMemoryLENGTH + 3] = SCALE(Player.poise, 0, 120);
        input[DistanceMemoryLENGTH + 4] = SCALE(PoiseDamageForAttack(Enemy.r_weapon_id, 46), 0, 80);
        for (int i = 0; i < AIHPMemoryLENGTH; i++){
            input[i + DistanceMemoryLENGTH + 5] = SCALE(AIHPMemory[i], 0, 2000);
        }
        input[DistanceMemoryLENGTH + 5 + AIHPMemoryLENGTH] = SCALE(Player.stamina, -40, 192);
        for (int i = 0; i < last_animation_types_enemy_LENGTH; i++){
            input[i + DistanceMemoryLENGTH + 5 + AIHPMemoryLENGTH + 1] = SCALE(last_animation_types_enemy[i], 0, 255);
        }
        input[DistanceMemoryLENGTH + 5 + AIHPMemoryLENGTH + 1 + last_animation_types_enemy_LENGTH] = SCALE(Player.bleedStatus, 0, 255);

        fann_type* out = fann_run(attack_mind_input->mind, input);

		//potentally move up if not in range
		if (mostRecentDistance > Player.weaponRange){
			AttackChoice = MoveUpId;
		}

		//TODO desicion about going for a backstab. Note that these subroutines will attempt, not garuntee
		//AttackChoice = PivotBSId;

		//TODO chain bs's. if enemy in bs, try chain

		//Decision about standard attack
		printf("Making attack decision");
		printf("Player weapon range = %f\n", Player.weaponRange);
		printf("Most recent distance = %f\n", mostRecentDistance);
		printf("Neural Network Result = %f\n", *out);
		printf("Player Stamina = %d\n", Player.stamina);
		printf("Player Bleed Status = %d\n", Player.bleedStatus);
		printf("Enemy Subanimation = %d\n", Enemy.subanimation);
		printf("Player weapon type = %d\n", Player.WeaponRoutines);
		if (
			!BackstabMetaOnly &&
			//sanity checks
			(mostRecentDistance + .05) <= Player.weaponRange && //in range
			(mostRecentDistance + .05) >= Player.minimumRange && //outside minimum range
			Player.stamina > 20 && //just to ensure we have enough to roll
			Player.bleedStatus > 40 && //more than one attack to proc bleed
			//static checks for attack
			((
			(Player.stamina > 90) && //safety buffer for stamina
				(Enemy.subanimation >= LockInSubanimation && Enemy.subanimation < SubanimationNeutral)  //enemy in vulnerable state, and can't immediatly transition
				) ||
				(*out > 0.5)//neural network says so
				))
		{
			printf("Neural Network Result = %f\n", *out);

			//if enemy is kicked, react based on weapon type
			if ((Enemy.passiveState_id == 51) || (Enemy.passiveState_id == 56)) {
				switch (Player.WeaponRoutines)
				{
				case 3:
					AttackChoice = PivotBSId;
					break;
				case 5:
					AttackChoice = PivotBSId;
					break;
				case 12:
					AttackChoice = PivotBSId;
					break;
				case 16:
					AttackChoice = PivotBSId;
					break;
				case 17:
					AttackChoice = PivotBSId;
					break;
				default:
					guiPrint(LocationState",1:r1");
					AttackChoice = GhostHitId;
					break;
				}
			}
			//randomly choose offensive options based on weapon
			//throw off enemy predictions
			//A lot of situations will end up in a Ghost Strike since all weaps can use it, but I tried to leave room for a few options each case
			else {
				int weaponRandom = rand() % 5;
				switch (weaponRandom) {
				case 0: //Ghost strike
					if (Player.isSpellTool == 0) {
						AttackChoice = GhostHitId;
					}
					else {
						AttackChoice = CastCancelId;
					}
				break;
				case 1: //Kick if in range unless CS or Thrusting, else DA or ghost
					if ((mostRecentDistance <= 2.8) && ((Player.WeaponRoutines >= 8 && Player.WeaponRoutines <= 9) || Player.WeaponRoutines == 11)) {
						AttackChoice = KickId;
					}
					else if (mostRecentDistance <= 1.9 && Player.WeaponRoutines == 13) {
						AttackChoice = KickId;
					}
					else if (mostRecentDistance <= 2.5 && Player.WeaponRoutines != 4 && Player.WeaponRoutines != 7 && Player.isSpellTool == 0) {
						AttackChoice = KickId;
					}
					else if (Player.WeaponRoutines == 4) {
						AttackChoice = DeadAngleId;
					}
					else if (Player.isSpellTool >= 1) {
						AttackChoice = CastSpellId;
					}
					else {
						AttackChoice = GhostHitId;
					}
				break;
				case 2: //DA if possible, else Ghost
					if (Player.WeaponRoutines == 3 || Player.WeaponRoutines == 4 || (!Player.twoHanding && Player.WeaponRoutines == 2)) {
						AttackChoice = DeadAngleId;
					}
					else if (Player.isSpellTool >= 1) {
						AttackChoice = CastCancelId;
					}
					else {
						AttackChoice = GhostHitId;
					}
				break;
				case 3: //cast if possible, else kick if in range, else ghost
					if (Player.isSpellToolOff >= 1 && (mostRecentDistance + .05 <= Player.spellRange)) {
						AttackChoice = CastSpellOffId;
					}
					else if (Player.isSpellToolOff >= 1) {
						AttackChoice = CastCancelOffId;
					}
					else if (Player.isSpellTool >= 1) {
						AttackChoice = CastSpellId;
					}
					else if ((mostRecentDistance <= 2.8) && ((Player.WeaponRoutines >= 8 && Player.WeaponRoutines <= 9) || Player.WeaponRoutines == 11)) {
						AttackChoice = KickId;
					}
					else if (mostRecentDistance <= 1.9 && Player.WeaponRoutines == 13) {
						AttackChoice = KickId;
					}
					else if (mostRecentDistance <= 2.5 && Player.isSpellTool == 0) {
						AttackChoice = KickId;
					}
					else {
						AttackChoice = GhostHitId;
					}
				break;
				case 4: //shieldpoke if thrusting sword or spear in range and have a shield in left hand, else ghost
					if ((mostRecentDistance + .05) <= (Player.weaponRange - 1.2) && (Player.WeaponRoutines == 7 || Player.WeaponRoutines == 14) && !Player.twoHanding && IsWeaponShield(Player.l_weapon_id)) {
						AttackChoice = ShieldPokeId;
					}
					else if (Player.isSpellTool >= 1) {
						AttackChoice = CastSpellId;
					}
					else {
						AttackChoice = GhostHitId;
					}
				}
			}
		}
		/*else if (
			!BackstabMetaOnly &&
			//sanity checks
			(mostRecentDistance <= 2) && //in range
			Player.stamina > 20 && //just to ensure we have enough to roll
			Player.bleedStatus > 40 && //more than one attack to proc bleed
									   //static checks for attack
			((
			(Player.stamina > 90) && //safety buffer for stamina
				(Enemy.subanimation == SubanimationNeutral)  //enemy in neutral state
				) ||
				(*out > 0.5)//neural network says so
				))
		{
			switch (Player.WeaponRoutines) {
				printf("Enemy neutral, kick if possible");
			case 4:
				AttackChoice = DeadAngleId;
				break;
			case 7:
				AttackChoice = PivotBSId;
				break;
			default:
				AttackChoice = KickId;
				break;
			}
		}*/

        //prevent rerun
        attack_mind_input->runNetwork = false;
        //release lock
        LeaveCriticalSection(&(attack_mind_input->crit));
        WakeConditionVariable(&(attack_mind_input->cond));
    }
    return 0;
}

//Helper Methods

int ReadyThreads(){
    //Defense Thread
    defense_mind_input = malloc(sizeof(MindInput));
	struct fann* defense_mind = fann_create_from_file("E:/unda/Documents/Bot King/Neural Nets/Defense_dark_souls_ai.net");
    if (defense_mind == NULL){
        printf("Defense_dark_souls_ai.net neural network file not found");
        return EXIT_FAILURE;
    }
    defense_mind_input->mind = defense_mind;
    defense_mind_input->exit = false;
    InitializeConditionVariable(&(defense_mind_input->cond));
    InitializeCriticalSection(&(defense_mind_input->crit));
    EnterCriticalSection(&(defense_mind_input->crit));
    defense_mind_input->runNetwork = false;
    HANDLE* defense_mind_thread = CreateThread(NULL, 0, DefenseMindProcess, NULL, 0, NULL);

    //Attack Thread
    attack_mind_input = malloc(sizeof(MindInput));
	struct fann* attack_mind = fann_create_from_file("E:/unda/Documents/Bot King/Neural Nets/Attack_dark_souls_ai.net");
    if (attack_mind == NULL){
        printf("Attack_dark_souls_ai.net neural network file not found");
        return EXIT_FAILURE;
    }
    attack_mind_input->mind = attack_mind;
    attack_mind_input->exit = false;
    InitializeConditionVariable(&(attack_mind_input->cond));
    InitializeCriticalSection(&(attack_mind_input->crit));
    EnterCriticalSection(&(attack_mind_input->crit));
    attack_mind_input->runNetwork = false;
    HANDLE* attack_mind_thread = CreateThread(NULL, 0, AttackMindProcess, NULL, 0, NULL);

    return 0;
}

void WaitForThread(MindInput* input){
    //get control of lock
    EnterCriticalSection(&(input->crit));
    //wait for neural net thread to mark self as finished
    while (input->runNetwork == true){
        bool result = SleepConditionVariableCS(&(input->cond), &(input->crit), 10);
        if (!result){
            guiPrint(LocationDetection",2:Timeout in reaquiring thread");
            break;
        }
    }
}

void WakeThread(MindInput* input){
    //trigger threads to run
    input->runNetwork = true;
    //release lock
    LeaveCriticalSection(&(input->crit));
    //wake thread
    WakeConditionVariable(&(input->cond));
}