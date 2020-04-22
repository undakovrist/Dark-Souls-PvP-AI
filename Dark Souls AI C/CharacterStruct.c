#include "CharacterStruct.h"
#define PI 3.14159265

#pragma warning( disable: 4244 )//ignore dataloss conversion from double to float
#pragma warning( disable: 4305 )

ullong Enemy_base_add = 0x00F7DC70;
ullong player_base_add = 0x00F7D644;
ullong spell_base_add = 0x00F7CE14;

//NOTE: this is curently hardcoded until i find a dynamic way
//How To Find: Increase this value until the attack ends with the AI turned away from the enemy. Decrease till it doesnt.
float WeaponGhostHitTime;

static bool waitingForAnimationTimertoCatchUp = false;

void ReadPlayer(Character * c, HANDLE processHandle, int characterId){
    //TODO read large block that contains all data, then parse in process
    //read x location
    ReadProcessMemory(processHandle, (LPCVOID)(c->location_x_address), &(c->loc_x), 4, 0);
    guiPrint("%d,0:X:%f", characterId, c->loc_x);
    //read y location
    ReadProcessMemory(processHandle, (LPCVOID)(c->location_y_address), &(c->loc_y), 4, 0);
    guiPrint("%d,1:Y:%f", characterId, c->loc_y);
    //read rotation of player
    ReadProcessMemory(processHandle, (LPCVOID)(c->rotation_address), &(c->rotation), 4, 0);
    //Player rotation is pi. 0 to pi,-pi to 0. Same as atan2
    //convert to radians, then to degrees
    c->rotation = (c->rotation + PI) * (180.0 / PI);
    guiPrint("%d,2:Rotation:%f", characterId, c->rotation);
    //read current animation type
    ReadProcessMemory(processHandle, (LPCVOID)(c->animationType_address), &(c->animationType_id), 2, 0);
    guiPrint("%d,3:Animation Type:%d", characterId, c->animationType_id);
    //remember enemy animation types
    if (characterId == EnemyId){
        AppendAnimationTypeEnemy(Enemy.animationType_id);
    }
	//read current passiveStateId
	ReadProcessMemory(processHandle, (LPCVOID)(c->passiveState_address), &(c->passiveState_id), 2, 0);
	guiPrint("%d,3:Animation Type:%d", characterId, c->passiveState_id);
	//remember enemy passiveStateIds
	if (characterId == EnemyId) {
		AppendpassiveStateEnemy(Enemy.passiveState_id);
	}
    //read hp
    ReadProcessMemory(processHandle, (LPCVOID)(c->hp_address), &(c->hp), 4, 0);
    guiPrint("%d,4:HP:%d", characterId, c->hp);
    if (characterId == PlayerId){
        AppendAIHP(Player.hp);
    }
    //read stamina
    if (c->stamina_address){
        ReadProcessMemory(processHandle, (LPCVOID)(c->stamina_address), &(c->stamina), 4, 0);
        guiPrint("%d,5:Stamina:%d", characterId, c->stamina);
    }
    //read what weapon they currently have in right hand
    ReadProcessMemory(processHandle, (LPCVOID)(c->r_weapon_address), &(c->r_weapon_id), 4, 0);
    guiPrint("%d,6:R Weapon:%d", characterId, c->r_weapon_id);

	//read weapon type and set variable for switch case for choosing attack options
	//most of these will assume you're using a "fast" weapon in each class. May not matter for curved/thrusting swords. May account for slow weaps later, idk
	// Daggers = 0
	if ((c->r_weapon_id >= 100000 && c->r_weapon_id <= 104950) || (c->r_weapon_id >= 9011000 && c->r_weapon_id <= 9011950)) {
		c->weaponRange = 2.4;
		if (characterId == PlayerId) {
			c->WeaponRoutines = 0;
			c->minimumRange = 0;
			WeaponGhostHitTime = 0.16;
		}
		else if (characterId == EnemyId && (c->animationType_id == R2_1H || c->animationType_id == R2_1H_Combo1 || c->animationType_id == R2_2H || c->animationType_id == R2_2H_Combo1)) {
			c->weaponRange = 3.1; //Handles all dagger R2s other than DST, but that's slow anyway so just attack through it
		}
	} // Straight swords = 1 Maybe make a separate one for Longsword as well?
	else if (c->r_weapon_id >= 200000 && c->r_weapon_id <= 212950) {
		c->weaponRange = 3.5; //Mostly for BSS and SKSS. Based on 1h r1s
		if (characterId == PlayerId) {
			c->WeaponRoutines = 1;
			c->minimumRange = 0;
			WeaponGhostHitTime = 0.22;
		}
		else if (characterId == EnemyId) {
			if (c->animationType_id == R2_1H || c->animationType_id == R2_1H_Combo1 || c->animationType_id == R2_2H || c->animationType_id == R2_2H_Combo1) {
				c->weaponRange = 4.5; //BSS 1h r2 is longest and also strong so we use that
			}
			else if (c->animationType_id == Kick_1H || c->animationType_id == Kick_2H) {
				c->weaponRange = 2.5;
			}
		}
	} // Greatswords = 2
	else if ((c->r_weapon_id >= 300000 && c->r_weapon_id <= 315950) || (c->r_weapon_id >= 9012000 && c->r_weapon_id <= 9013950) || (c->r_weapon_id >= 9020000 && c->r_weapon_id <= 9020950)) {
		c->weaponRange = 3.9; //Based on 2h r1s
		if (characterId == PlayerId) {
			c->WeaponRoutines = 2;
			c->minimumRange = 0;
			WeaponGhostHitTime = 0.26;
		}
		else if (characterId == EnemyId) {
			if (c->animationType_id == R2_1H || c->animationType_id == R2_1H_Combo1 || c->animationType_id == R2_2H || c->animationType_id == R2_2H_Combo1) {
				c->weaponRange = 6.1; //GS of Artorias 1h r2 range. You will almost certainly never need this, but Abyss GS is even further and you def don't need that ever
			}
			else if (c->animationType_id == Kick_1H || c->animationType_id == Kick_2H) {
				c->weaponRange = 2.5;
			}
		}
	} // Ultra-Greatswords = 3
	else if (c->r_weapon_id >= 350000 && c->r_weapon_id <= 355950) {
		c->weaponRange = 3.2; //Based on 2h r1s
		if (characterId == PlayerId) {
			c->WeaponRoutines = 3;
			c->minimumRange = 0;
			WeaponGhostHitTime = 0.52;
		}
		else if (characterId == EnemyId) {
			if (c->animationType_id == R2_1H || c->animationType_id == R2_1H_Combo1 || c->animationType_id == R2_2H || c->animationType_id == R2_2H_Combo1) {
				c->weaponRange = 5.5;
			}
			else if (c->animationType_id == Kick_1H || c->animationType_id == Kick_2H) {
				c->weaponRange = 2.5;
			}
		}
	} // Curved Sword = 4
	else if ((c->r_weapon_id >= 400000 && c->r_weapon_id <= 406505) || (c->r_weapon_id >= 9010000 && c->r_weapon_id <= 9010950)) {
		if (c->r_weapon_id >= 406500 && c->r_weapon_id <= 406505) { //Based on 2h r1s
			c->weaponRange = 3.6; //QFS
		}
		else {
			c->weaponRange = 3.1; //Mostly Falchion and GT
		}
		if (characterId == PlayerId) {
			c->WeaponRoutines = 4;
			c->minimumRange = 0;
			WeaponGhostHitTime = 0.22;
		}
		else if (characterId == EnemyId) {
			if (c->animationType_id == R2_1H || c->animationType_id == R2_1H_Combo1 || c->animationType_id == R2_2H || c->animationType_id == R2_2H_Combo1) {
				c->weaponRange = 3.6; //Shotel 2h r2 is longest and strong so we use that
			}
			else if (c->animationType_id == Kick_1H || c->animationType_id == Kick_2H) {
				c->weaponRange = 2.65;
			}
		}
	} // Curved Greatswords = 5
	else if (c->r_weapon_id >= 450000 && c->r_weapon_id <= 453950) {
		c->weaponRange = 3.5; //For Kumo, but honestly GLS isn't much shorter so that should be fine too.
		if (characterId = PlayerId) {
			c->WeaponRoutines = 5;
			c->minimumRange = 0;
			WeaponGhostHitTime = 0.6;
		}
		else if (characterId == EnemyId) {
			if (c->animationType_id == R2_1H || c->animationType_id == R2_1H_Combo1 || c->animationType_id == R2_2H || c->animationType_id == R2_2H_Combo1) {
				c->weaponRange = 5.25;
			}
			else if (c->animationType_id == Kick_1H || c->animationType_id == Kick_2H) {
				c->weaponRange = 2.5;
			}
		}
	} // Katanas = 6
	else if (c->r_weapon_id >= 500000 && c->r_weapon_id <= 503950) { //Range is important here so I'll have to split them up.
		if (c->r_weapon_id >= 501000 && c->l_weapon_id <= 501950) { //WP
			c->weaponRange = 3.9;
		}
		else if (c->r_weapon_id >= 503000 && c->l_weapon_id <= 503950) { //CB
			c->weaponRange = 3.65;
		}
		else {
			c->weaponRange = 3.5; //Uchi & Iaito
		}
		if (characterId == PlayerId) {
			c->WeaponRoutines = 6;
			c->minimumRange = 0;
			WeaponGhostHitTime = 0.23;
		}
		else if (characterId == EnemyId){
			if (c->animationType_id == R2_1H || c->animationType_id == R2_1H_Combo1 || c->animationType_id == R2_2H || c->animationType_id == R2_2H_Combo1) {
			c->weaponRange = 5.75; //You're not using katana r2s unless it's Uchi or WP usually, so we gotta avoid that 1h r2
			}
			else if (c->animationType_id == Kick_1H || c->animationType_id == Kick_2H) {
				c->weaponRange = 2.5;
			}
		}
	} // Thrusting Swords = 7
	else if (c->r_weapon_id >= 600000 && c->r_weapon_id <= 604950) {
		if (c->r_weapon_id >= 602000 && c->r_weapon_id <= 602950) {
			c->weaponRange = 3.9; //Estoc
		}
		else if (c->r_weapon_id >= 603000 && c->r_weapon_id <= 604950) {
			c->weaponRange = 3.7; //Velka's and Ricard's
		}
		else {
			c->weaponRange = 3.5; //Assumes Rapier since Mail Breaker is terrible
		}
		if (characterId == PlayerId) {
			c->WeaponRoutines = 7;
			c->minimumRange = 2;
			WeaponGhostHitTime = 0.28;
		}
		else if (characterId == EnemyId ) {
			if (c->animationType_id == R2_1H || c->animationType_id == R2_1H_Combo1 || c->animationType_id == R2_2H || c->animationType_id == R2_2H_Combo1) {
				c->weaponRange = 4.9;
			}
			else if (c->animationType_id == Kick_1H || c->animationType_id == Kick_2H) {
				c->weaponRange = 2.6;
			}
		}
	} // Hand Axe = 8 (Too different from normal axes)
	else if (c->r_weapon_id >= 700000 && c->r_weapon_id <= 700950) {
		c->weaponRange = 3.25;
		if (characterId == PlayerId) {
			c->WeaponRoutines = 8;
			c->minimumRange = 0;
			WeaponGhostHitTime = 0.16;
		} //Handaxe r2s are about the same length as its r1s so no issues here
	} // Axes = 9
	else if (c->r_weapon_id >= 701000 && c->r_weapon_id <= 705950) {
		if (c->r_weapon_id >= 701000 && c->r_weapon_id <= 701950) {
			c->weaponRange = 3.3; //Battle Axe
		}
		else if (c->r_weapon_id >= 704000 && c->r_weapon_id <= 704950) {
			c->weaponRange = 3.3; //Golem Axe
		}
		else { 
			c->weaponRange = 3.75;  //Crescent axe, Gargoyle Tail Axe, Butcher's Knife
		}
		if (characterId == PlayerId) {
			c->WeaponRoutines = 9;
			c->minimumRange = 0;
			WeaponGhostHitTime = 0.20;
		}
		else if (characterId == EnemyId) {
			if (c->animationType_id == R2_1H || c->animationType_id == R2_1H_Combo1 || c->animationType_id == R2_2H || c->animationType_id == R2_2H_Combo1) {
				c->weaponRange = 3.75; //The only longer r2 is Butcher Knife 1h r2. By a lot. But they're way more likely to just run past you.
			}
			else if (c->animationType_id == Kick_1H || c->animationType_id == Kick_2H) {
				c->weaponRange = 2.8;
			}
		}
	} // BKGA = 10 (Other greataxes are just way too bad to bother with, and very different)
	else if (c->r_weapon_id >= 753000 && c->r_weapon_id <= 753950) {
		c->weaponRange = 3.45;
		if (characterId == PlayerId) {
			c->WeaponRoutines = 10;
			c->minimumRange = 0;
			WeaponGhostHitTime = 0.26;
		} //BKGA 2hr2 is barely longer than 2h r1 so not bothering. 1h will likely miss every time.
		else if (characterId == EnemyId && (c->animationType_id == Kick_1H || c->animationType_id == Kick_2H)) {
			c->weaponRange = 2.5;
		}
	} // Hammers = 11
	else if (c->r_weapon_id >= 800000 && c->r_weapon_id <= 812950) {
		if (c->r_weapon_id >= 811000 && c->r_weapon_id <= 811950) {
			c->weaponRange = 3.6; //Blacksmith Giant Hammer since that's all anyone ever uses
		}
		else {
			c->weaponRange = 3; //catch-all for every other hammer
		}
		if (characterId == PlayerId) {
			c->WeaponRoutines = 11;
			c->minimumRange = 0;
			WeaponGhostHitTime = 0.43;
		}
		else if (characterId == EnemyId) {
			if (c->animationType_id == R2_1H || c->animationType_id == R2_1H_Combo1 || c->animationType_id == R2_2H || c->animationType_id == R2_2H_Combo1) {
				c->weaponRange = 3.75; //Warpick r2s. The jumping r2s of some are more likely to just miss
			}
			else if (c->animationType_id == Kick_1H || c->animationType_id == Kick_2H) {
				c->weaponRange = 2.8;
			}
		}
	} // Great Hammers = 12 (This one is just bad cause I don't even know what to do with them)
	else if (c->r_weapon_id >= 850000 && c->r_weapon_id <= 857950) {
		c->weaponRange = 4.25; // Large Club and bigger
		if (characterId == PlayerId) {
			c->WeaponRoutines = 12;
			c->minimumRange = 0;
			WeaponGhostHitTime = 0.5; //random guess tbh
		}
		else if (characterId == EnemyId) {
			if (c->animationType_id == R2_1H || c->animationType_id == R2_1H_Combo1 || c->animationType_id == R2_2H || c->animationType_id == R2_2H_Combo1)
				c->weaponRange = 5.05;
			else if (c->animationType_id == Kick_1H || c->animationType_id == Kick_2H) {
				c->weaponRange = 2.5;
			}
		}
	} // Fist Weapons = 13 uhhhh I guess
	else if (c->r_weapon_id >= 900000 && c->r_weapon_id <= 904950) {
		c->weaponRange = 2.85;
		if (characterId == PlayerId) {
			c->WeaponRoutines = 13;
			c->minimumRange = 0;
			WeaponGhostHitTime = 0.2;
		}
		else if (characterId == EnemyId) {
			if (c->animationType_id == R2_1H || c->animationType_id == R2_1H_Combo1 || c->animationType_id == R2_2H || c->animationType_id == R2_2H_Combo1) {
				c->weaponRange = 3.85;
			}
			else if (c->animationType_id == Kick_1H || c->animationType_id == Kick_2H) {
				c->weaponRange = 1.9;
			}
		}
	} // All Spears = 14  (Oh boy there's too many spears with different ranges) Based around 2h r1s
	else if ((c->r_weapon_id >= 1000000 && c->r_weapon_id <= 1006950) || (c->r_weapon_id >= 1050000 && c->r_weapon_id <= 1054950) || (c->r_weapon_id >= 9016000 && c->r_weapon_id <= 9016950)) {
		//Demon's Spear
		if (c->r_weapon_id >= 1003000 && c->r_weapon_id <= 1003005) {
			c->weaponRange = 5.4;
			if (characterId == EnemyId) {
				if (c->animationType_id == R2_1H || c->animationType_id == R2_1H_Combo1 || c->animationType_id == R2_2H || c->animationType_id == R2_2H_Combo1) {
					c->weaponRange = 7.85;
				}
				else if (c->animationType_id == Kick_1H || c->animationType_id == Kick_2H) {
					c->weaponRange = 2.5;
				}
			}
		} //Pike
		else if (c->r_weapon_id >= 1005000 && c->r_weapon_id <= 1005905) {
			c->weaponRange = 4.7;
			if (characterId == EnemyId && (c->animationType_id == Kick_1H || c->animationType_id == Kick_2H)) {
				c->weaponRange = 2.5;
			}
		} //Silver Knight Spear & Dragonslayer Spear
		else if ((c->r_weapon_id >= 1006000 && c->r_weapon_id <= 1006950) || (c->r_weapon_id >= 1054000 && c->r_weapon_id <= 1054950)) {
			c->weaponRange = 5.1;
			if (characterId == EnemyId) {
				if (c->animationType_id == R2_1H || c->animationType_id == R2_1H_Combo1 || c->animationType_id == R2_2H || c->animationType_id == R2_2H_Combo1) {
					c->weaponRange = 5.9;
				}
				else if (c->animationType_id == Kick_1H || c->animationType_id == Kick_2H) {
					c->weaponRange = 2.5;
				}
			}
		} // Moonlight Butterfly Horn
		else if (c->r_weapon_id >= 1052000 && c->r_weapon_id <= 1053905) {
			c->weaponRange = 4.95;
			if (characterId == EnemyId) {
				if (c->animationType_id == R2_1H || c->animationType_id == R2_1H_Combo1 || c->animationType_id == R2_2H || c->animationType_id == R2_2H_Combo1) {
					c->weaponRange = 5.6;
				}
				else if (c->animationType_id == Kick_1H || c->animationType_id == Kick_2H) {
					c->weaponRange = 2.5;
				}
			}
		}
		else { //Winged, Partizan, Channeler's Trident, and Four-Pronged Plow. Ignoring Spear cause bad
			c->weaponRange = 4.25;
			if (characterId == EnemyId) {
				if (c->animationType_id == R2_1H || c->animationType_id == R2_1H_Combo1 || c->animationType_id == R2_2H || c->animationType_id == R2_2H_Combo1) {
					c->weaponRange = 4.95;
				}
				else if (c->animationType_id == Kick_1H || c->animationType_id == Kick_2H) {
					c->weaponRange = 2.5;
				}
			}
		}
		if (characterId == PlayerId) {
			c->WeaponRoutines = 14;
			c->minimumRange = 3;
			WeaponGhostHitTime = 0.3;
		}
	}
		 // Halberd = 15 (Different from other halberds)
	else if (c->r_weapon_id >= 1100000 && c->r_weapon_id <= 1100950) {
		c->weaponRange = 4.35;
		if (characterId == PlayerId) {
			c->WeaponRoutines = 15;
			c->minimumRange = 3;
			WeaponGhostHitTime = 0.28;
		}
		else if (characterId == EnemyId && (c->animationType_id == Kick_1H || c->animationType_id == Kick_2H)) {
				c->weaponRange = 2.5;
		}
	} // Other Halberds = 16 (They have bad neutral and I'm lazy so they're getting the one size fits all)
	else if (c->r_weapon_id >= 1101000 && c->r_weapon_id <= 1107950) {
		c->weaponRange = 4.35;
		if (characterId == PlayerId) {
			c->WeaponRoutines = 16;
			c->minimumRange = 0;
			WeaponGhostHitTime = 0.28;
		}
		if (characterId == EnemyId) {
			if (c->animationType_id == R2_1H || c->animationType_id == R2_1H_Combo1 || c->animationType_id == R2_2H || c->animationType_id == R2_2H_Combo1) {
				c->weaponRange = 5.85;
			}
			else if (c->animationType_id == Kick_1H || c->animationType_id == Kick_2H) {
				c->weaponRange = 2.5;
			}
		}
	} // Scythes = 17 (Again, bad neutral and I haven't implemented r2s, running, rolling, so screw em)
	else if (c->r_weapon_id >= 1150000 && c->r_weapon_id <= 1151950) {
		c->weaponRange = 3.5;
		if (characterId == PlayerId) {
			c->WeaponRoutines = 17;
			c->minimumRange = 0;
			WeaponGhostHitTime = 0.46;
		}
		else if(characterId == EnemyId) {
			if (c->animationType_id == R2_1H || c->animationType_id == R2_1H_Combo1 || c->animationType_id == R2_2H || c->animationType_id == R2_2H_Combo1) {
				c->weaponRange = 5.8;
			}
			else if (c->animationType_id == Kick_1H || c->animationType_id == Kick_2H) {
				c->weaponRange = 2.5;
			}
		}
	} 
	else if (c->r_weapon_id >= 1330000 && c->r_weapon_id <= 1332950) { //Pyro Flame
		c->weaponRange = 3.5; // Default range to GC/BF so bot knows when to cast it if equipped on him
		if (characterId == PlayerId) {
			c->isSpellTool = 1;
		}
		else if (characterId == EnemyId) {
			if (c->animationType_id == FireBall_Cast || c->animationType_id == FireStorm_Cast || c->animationType_id == FireSurge_Cast_RH) {
				c->weaponRange == 7;
			}
		}
	}
	else if (c->r_weapon_id >= 1360000 && c->r_weapon_id <= 1367000) { //Talisman
		c->weaponRange = 4.85;
		if (characterId == PlayerId) {
			c->isSpellTool = 2;
		}
		else if (characterId == EnemyId) {
			if (c->animationType_id == Miricle_Projectile_Cast || c->animationType_id == Miricle_Throw_Cast)
				c->weaponRange = 7;
		}
	}
	else if (c->r_weapon_id >= 1300000 && c->r_weapon_id <= 1308000) { //Catalysts (currently unused)
		c->weaponRange = 7;
		if (characterId == PlayerId) {
			c->isSpellTool = 3;
		}
	}
	else { // If for some reason it's not found
		c->weaponRange = 6;
		if (characterId == PlayerId) {
			c->WeaponRoutines = 13;
			c->minimumRange = 0;
			WeaponGhostHitTime = 0.22;
		}
	}

    //read what weapon they currently have in left hand
    ReadProcessMemory(processHandle, (LPCVOID)(c->l_weapon_address), &(c->l_weapon_id), 4, 0);
    guiPrint("%d,7:L Weapon:%d", characterId, c->l_weapon_id);
	//read if a spell tool or a (good) shield is currently equipped and set another switch case flag
	if (c->l_weapon_id >= 1330000 && c->l_weapon_id <= 1332950) { //Pyro Flame
		c->spellRange = 3.5; // Default range to GC/BF so bot knows when to cast it if equipped on him
		if (characterId == PlayerId) {
			c->isSpellToolOff = 1;
		}
		else if (characterId == EnemyId) {
			if (c->animationType_id == FireBall_Cast || c->animationType_id == FireStorm_Cast || c->animationType_id == FireSurge_Cast_RH) {
				c->spellRange == 7;
			}
		}
	}
	else if (c->l_weapon_id >= 1360000 && c->l_weapon_id <= 1367000) { //Talisman
		c->spellRange = 4.85;
		if (characterId == PlayerId) {
			c->isSpellToolOff = 2;
		}
		else if (characterId == EnemyId) {
			if (c->animationType_id == Miricle_Projectile_Cast || c->animationType_id == Miricle_Throw_Cast)
				c->weaponRange = 7;
		}
	}
	else if (c->l_weapon_id >= 1300000 && c->l_weapon_id <= 1308000) { //Catalysts (currently unused)
		c->weaponRange = 7;
		if (characterId == PlayerId) {
			c->isSpellToolOff = 3;
		}
	}
	/*else if (c->l_weapon_id >= 1474000 && c->l_weapon_id <= 1474950) { //Black Knight Shield
		Player.DefendRoutines = 1;
	}*/


    //read if hurtbox is active on enemy weapon
    if (c->hurtboxActive_address){
        unsigned char hurtboxActiveState;
        ReadProcessMemory(processHandle, (LPCVOID)(c->hurtboxActive_address), &hurtboxActiveState, 1, 0);
        if (hurtboxActiveState){
            c->subanimation = AttackSubanimationActiveDuringHurtbox;
        }
    }

	//Check enemies weapon type. If a fast weap, will disable toggle escapes
	//Daggers/Straight Swords
	if (Enemy.r_weapon_id >= 100000 && Enemy.r_weapon_id <= 212950) {
		EnemyWeaponClass = 0;
	} // Curved Sword
	else if (Enemy.r_weapon_id >= 400000 && Enemy.r_weapon_id <= 406950) {
		EnemyWeaponClass = 0;
	} // Katanas, Thrusting Swords, Hand Axe
	else if (Enemy.r_weapon_id >= 500000 && Enemy.r_weapon_id <= 700950) {
		EnemyWeaponClass = 0;
	} // Fist Weapons/Spears
	else if (Enemy.r_weapon_id >= 900000 && Enemy.r_weapon_id <= 1054950) {
		EnemyWeaponClass = 0;
	} // GT/DST
	else if (Enemy.r_weapon_id >= 9010000 && Enemy.r_weapon_id <= 9011950) {
		EnemyWeaponClass = 0;
	} // Four-Pronged Plow
	else if (Enemy.r_weapon_id >= 9016000 && Enemy.r_weapon_id <= 9016950) {
		EnemyWeaponClass = 0;
	} // GS/UGS/CGS/Axe/GA/Hammer/GH/Halb can be toggled
	else {
		EnemyWeaponClass = 1;
	}

	//check for a good pyro block shield (DCS, BKS, Greatshields
	/*if ((Player.l_weapon_id >= 1450000 && Player.l_weapon_id <= 1452950) || (Player.l_weapon_id >= 1450000 && Player.l_weapon_id <= 1452950)) {
	
	}*/

    int animationid;
    ReadProcessMemory(processHandle, (LPCVOID)(c->animationId_address), &animationid, 4, 0);
    //need a second one b/c the game has a second one. the game has a second one b/c two animations can overlap.
    int animationid2;
    ReadProcessMemory(processHandle, (LPCVOID)(c->animationId2_address), &animationid2, 4, 0);
	//haven't discovered what the 3rd animation address is for besides backstabs
	int animationid3;
	ReadProcessMemory(processHandle, (LPCVOID)(c->animationId3_address), &animationid3, 4, 0);
	if (animationid3 > 0){
		c->in_backstab = 1;
	} else{
		c->in_backstab = 0;
	}

    //keep track of enemy animations in memory
    if (characterId == EnemyId){
        if (animationid != -1){
            waitingForAnimationTimertoCatchUp |= AppendLastAnimationIdEnemy(animationid);
        } else {
            waitingForAnimationTimertoCatchUp |= AppendLastAnimationIdEnemy(animationid2);
        }
    }

    guiPrint("%d,8:Animation Id 1/2:%d/%d", characterId, animationid, animationid2);

    unsigned char attackAnimationInfo = isAttackAnimation(c->animationType_id);

    //---any subanimation that is based purely off animation id should be prioritized in subanimation state setting---
    if (isVulnerableAnimation(animationid))
    {
        c->subanimation = LockInSubanimation;
    }
    else if (animationid >= 2000 && animationid <= 2056){//animation states for poise breaks, knockdowns, launches, staggers
        c->subanimation = PoiseBrokenSubanimation;
    }
    //---subanimations based on animation type---
    else if (isDodgeAnimation(c->animationType_id) && animationid != -1){//in theory these two should never conflict. In practice, one might be slow.
        c->subanimation = LockInSubanimation;
    }

    //read how long the animation has been active, check with current animation, see if hurtbox is about to activate
    //what i want is a countdown till hurtbox is active
    //cant be much higher b/c need spell attack timings
    //also check that this is an attack that involves subanimation
    else if (attackAnimationInfo == 2 || attackAnimationInfo == 4 || attackAnimationInfo == 5){
        int curAnimationTimer_address = 0;
        int curAnimationid = 0;

        //need a second one b/c the game has a second one. the game has a second one b/c two animations can overlap.
        if (animationid2 > 1000){
            curAnimationTimer_address = c->animationTimer2_address;
            curAnimationid = animationid2;
        }
        else if (animationid > 1000){
            //if kick or parry (aid ends in 100), use catch all aid
            if (animationid % 1000 == 100){
                curAnimationid = 100;
            } else{
                curAnimationid = animationid;
            }
            curAnimationTimer_address = c->animationTimer_address;
        }
        else{
            guiPrint(LocationDetection",3:ALERT: Animation type found but not animation ids");
        }

        if (curAnimationid){
            float animationTimer;
            ReadProcessMemory(processHandle, (LPCVOID)(curAnimationTimer_address), &animationTimer, 4, 0);

            //handle the timer not being reset to 0 as soon as a new animation starts
            //wait for animation timer to go below 0.1(a tell for its been reset, since no animation is short enought to have held it at 0.1), then we can stop manually resetting it
            if (waitingForAnimationTimertoCatchUp && animationTimer > 0.1){
                animationTimer = 0.0;
            } else{
                waitingForAnimationTimertoCatchUp = false;
            }

            //sometimes, due to lag, dark souls cuts one animation short and makes the next's hurtbox timing later. handle this for the animations that do it by treating the two animations as one.
			AnimationCombineReturn animationToCombine;
			CombineLastAnimation(curAnimationid, &animationToCombine);
            if (animationToCombine.animationId){
                curAnimationid = animationToCombine.animationId;//combine the two animations and treat as one id 
                if (animationToCombine.partNumber){
                    //this uses the fact that animation timers are not reset by the game after use
                    float animationTimer2;
                    ReadProcessMemory(processHandle, (LPCVOID)(c->animationTimer2_address), &animationTimer2, 4, 0);
                    animationTimer += animationTimer2;
                }
            }

            float dodgeTimer = dodgeTimings(curAnimationid);
            float timeDelta = dodgeTimer - animationTimer;
            c->dodgeTimeRemaining = timeDelta;

            if (timeDelta >= 1.0){
                c->subanimation = SubanimationNeutral;
            } else if (timeDelta < 1.0 && timeDelta > 0.55){
                c->subanimation = AttackSubanimationWindup;
            }
            //between 0.55 and 0.15 sec b4 hurtbox. If we have less that 0.15 we can't dodge.
            else if (timeDelta <= 0.55 && timeDelta >= 0.15){
                c->subanimation = AttackSubanimationWindupClosing;
            }
            //just treat this as the hurtbox is activated
            else if (timeDelta < 0.15 && timeDelta >= 0){
                c->subanimation = AttackSubanimationActiveDuringHurtbox;
            }
            else if (timeDelta < 0){
                c->subanimation = AttackSubanimationActiveHurtboxOver;
            }

            // time before the windup ends where we can still alter rotation (only for player)
			if (animationTimer > WeaponGhostHitTime && timeDelta >= -0.3 && characterId == PlayerId){
                c->subanimation = AttackSubanimationWindupGhostHit;
            }

            guiPrint("%d,9:Animation Timer:%f\nDodge Time:%f", characterId, animationTimer, dodgeTimer);
        }
    }
    else if (attackAnimationInfo == 1){
        c->subanimation = AttackSubanimationWindup;
    }
    else if (attackAnimationInfo == 3){
        c->subanimation = AttackSubanimationActiveDuringHurtbox;
    }
    else{
        //else if (c->animationType_id == 0){//0 when running, walking, standing. all animation can immediatly transition to new animation. Or animation id = -1
        c->subanimation = SubanimationNeutral;
    }

    //read if in ready state(can transition to another animation)
    if (c->readyState_address){
        unsigned char readyState;
        ReadProcessMemory(processHandle, (LPCVOID)(c->readyState_address), &readyState, 1, 0);
        if(readyState){
            c->subanimation = SubanimationRecover;
        } /*else{ Not adding this now because it would lock out subanimations every time i move
            c->subanimation = LockInSubanimation;
            }*/
    }
    guiPrint("%d,10:Subanimation:%d", characterId, c->subanimation);

    //read the current velocity
    //player doesnt use this, and wont have the address set. enemy will
    if (c->velocity_address){
        ReadProcessMemory(processHandle, (LPCVOID)(c->velocity_address), &(c->velocity), 4, 0);
        guiPrint("%d,11:Velocity:%f", characterId, c->velocity);
    }
    //read if the player is locked on
    if (c->locked_on_address){
        ReadProcessMemory(processHandle, (LPCVOID)(c->locked_on_address), &(c->locked_on), 1, 0);
        guiPrint("%d,12:Locked On:%d", characterId, c->locked_on);
    }
    //read two handed state of player
    if (c->twoHanding_address){
        ReadProcessMemory(processHandle, (LPCVOID)(c->twoHanding_address), &(c->twoHanding), 1, 0);
        guiPrint("%d,13:Two Handing:%d", characterId, c->twoHanding);
    }
    //read stamina recovery of enemy
    if (c->staminaRecoveryRate_address){
        ReadProcessMemory(processHandle, (LPCVOID)(c->staminaRecoveryRate_address), &(c->staminaRecoveryRate), 4, 0);
        guiPrint("%d,14:Stamina Recovery Rate:%d", characterId, c->staminaRecoveryRate);
    }
    //read current poise
    ReadProcessMemory(processHandle, (LPCVOID)(c->poise_address), &(c->poise), 4, 0);
    guiPrint("%d,15:Poise:%f", characterId, c->poise);
    //read current bleed status
    if (c->bleedStatus_address){
        ReadProcessMemory(processHandle, (LPCVOID)(c->bleedStatus_address), &(c->bleedStatus), 4, 0);
        guiPrint("%d,16:Bleed Status:%d", characterId, c->bleedStatus);
    }
}

void ReadPlayerDEBUGGING(Character * c, HANDLE * processHandle, ...){
    c->loc_x = 1045.967773;
    c->loc_y = 864.3547974;
    c->rotation = 360;//facing kinda towards bonfire, same as pi/-pi
    c->animationType_id = 4294967295;
    c->hp = 1800;
    c->r_weapon_id = 301015;
    c->l_weapon_id = 900000;
    c->subanimation = SubanimationNeutral;
    c->velocity = 0;
}

void ReadPointerEndAddresses(HANDLE processHandle){
    //add the pointer offsets to the address. This can be slow because its startup only
    Enemy.location_x_address = FindPointerAddr(processHandle, Enemy_base_add, Enemy_loc_x_offsets_length, Enemy_loc_x_offsets);
    Enemy.location_y_address = FindPointerAddr(processHandle, Enemy_base_add, Enemy_loc_y_offsets_length, Enemy_loc_y_offsets);
    Enemy.rotation_address = FindPointerAddr(processHandle, Enemy_base_add, Enemy_rotation_offsets_length, Enemy_rotation_offsets);
    Enemy.animationType_address = FindPointerAddr(processHandle, Enemy_base_add, Enemy_animationType_offsets_length, Enemy_animationType_offsets);
	Enemy.passiveState_address = FindPointerAddr(processHandle, Enemy_base_add, Enemy_passiveState_offsets_length, Enemy_passiveState_offsets);
    Enemy.hp_address = FindPointerAddr(processHandle, Enemy_base_add, Enemy_hp_offsets_length, Enemy_hp_offsets);
    Enemy.stamina_address = 0;
    Enemy.r_weapon_address = FindPointerAddr(processHandle, Enemy_base_add, Enemy_r_weapon_offsets_length, Enemy_r_weapon_offsets);
    Enemy.l_weapon_address = FindPointerAddr(processHandle, Enemy_base_add, Enemy_l_weapon_offsets_length, Enemy_l_weapon_offsets);
    Enemy.animationTimer_address = FindPointerAddr(processHandle, Enemy_base_add, Enemy_animationTimer_offsets_length, Enemy_animationTimer_offsets);
    Enemy.animationTimer2_address = FindPointerAddr(processHandle, Enemy_base_add, Enemy_animationTimer2_offsets_length, Enemy_animationTimer2_offsets);
    Enemy.animationId_address = FindPointerAddr(processHandle, Enemy_base_add, Enemy_animationID_offsets_length, Enemy_animationID_offsets);
    Enemy.animationId2_address = FindPointerAddr(processHandle, Enemy_base_add, Enemy_animationID2_offsets_length, Enemy_animationID2_offsets);
	Enemy.animationId3_address = FindPointerAddr(processHandle, Enemy_base_add, Enemy_animationID3_offsets_length, Enemy_animationID3_offsets);
    Enemy.hurtboxActive_address = FindPointerAddr(processHandle, Enemy_base_add, Enemy_hurtboxActive_offsets_length, Enemy_hurtboxActive_offsets);
    Enemy.readyState_address = 0;
    Enemy.velocity_address = FindPointerAddr(processHandle, Enemy_base_add, Enemy_velocity_offsets_length, Enemy_velocity_offsets);
    Enemy.locked_on_address = 0;
    Enemy.twoHanding_address = 0;
    Enemy.staminaRecoveryRate_address = FindPointerAddr(processHandle, Enemy_base_add, Enemy_stamRecovery_offsets_length, Enemy_stamRecovery_offsets);
    Enemy.poise_address = FindPointerAddr(processHandle, Enemy_base_add, Enemy_Poise_offsets_length, Enemy_Poise_offsets);
    Enemy.bleedStatus_address = 0;

    Player.location_x_address = FindPointerAddr(processHandle, player_base_add, Player_loc_x_offsets_length, Player_loc_x_offsets);
    Player.location_y_address = FindPointerAddr(processHandle, player_base_add, Player_loc_y_offsets_length, Player_loc_y_offsets);
    Player.rotation_address = FindPointerAddr(processHandle, player_base_add, Player_rotation_offsets_length, Player_rotation_offsets);
    Player.animationType_address = FindPointerAddr(processHandle, player_base_add, Player_animationType_offsets_length, Player_animationType_offsets);
	Player.passiveState_address = FindPointerAddr(processHandle, player_base_add, Player_passiveState_offsets_length, Player_passiveState_offsets);
    Player.hp_address = FindPointerAddr(processHandle, player_base_add, Player_hp_offsets_length, Player_hp_offsets);
    Player.stamina_address = FindPointerAddr(processHandle, player_base_add, Player_stamina_offsets_length, Player_stamina_offsets);
    Player.r_weapon_address = FindPointerAddr(processHandle, player_base_add, Player_r_weapon_offsets_length, Player_r_weapon_offsets);
    Player.l_weapon_address = FindPointerAddr(processHandle, player_base_add, Player_l_weapon_offsets_length, Player_l_weapon_offsets);
    Player.animationTimer_address = FindPointerAddr(processHandle, player_base_add, Player_animationTimer_offsets_length, Player_animationTimer_offsets);
    Player.animationTimer2_address = FindPointerAddr(processHandle, player_base_add, Player_animationTimer2_offsets_length, Player_animationTimer2_offsets);
    Player.animationId_address = FindPointerAddr(processHandle, player_base_add, Player_animationID_offsets_length, Player_animationID_offsets);
    Player.animationId2_address = FindPointerAddr(processHandle, player_base_add, Player_animationID2_offsets_length, Player_animationID2_offsets);
	Player.animationId3_address = FindPointerAddr(processHandle, player_base_add, Player_animationID3_offsets_length, Player_animationID3_offsets);
    Player.hurtboxActive_address = 0;
    Player.readyState_address = FindPointerAddr(processHandle, player_base_add, Player_readyState_offsets_length, Player_readyState_offsets);
	Player.velocity_address = 0;
   // Player.velocity_address = (processHandle, player_base_add, player_velocity_offsets_length, player_velocity_offsets);
    Player.locked_on_address = FindPointerAddr(processHandle, player_base_add, Player_Lock_on_offsets_length, Player_Lock_on_offsets);
    Player.twoHanding_address = FindPointerAddr(processHandle, player_base_add, Player_twohanding_offsets_length, Player_twohanding_offsets);
    Player.staminaRecoveryRate_address = 0;
    Player.poise_address = FindPointerAddr(processHandle, player_base_add, Player_Poise_offsets_length, Player_Poise_offsets);
    Player.bleedStatus_address = FindPointerAddr(processHandle, player_base_add, Player_BleedStatus_offsets_length, Player_BleedStatus_offsets);
}