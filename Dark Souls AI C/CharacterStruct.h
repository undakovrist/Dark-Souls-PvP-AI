#ifndef CharacterStruct_H
#define CharacterStruct_H

#include "gui.h"
#include <stdio.h>
#include <Windows.h>
#include "AnimationMappings.h"
#include "Memory.h"
#include "MemoryEdits.h"

typedef struct {
	//data for x location
	ullong location_x_address;
	float loc_x;
	//data for y location
	ullong location_y_address;
	float loc_y;
	//data for rotation
	ullong rotation_address;
	float rotation;
	//current animation type id
	ullong animationType_address;
    unsigned short animationType_id;
	//current passiveStateId
	ullong passiveState_address;
	unsigned short passiveState_id;
	//hp
	ullong hp_address;
	unsigned int hp;
    //stamina
    ullong stamina_address;
    int stamina;
	//current Right hand weapon they are holding
	ullong r_weapon_address;
	unsigned int r_weapon_id;
	//current left hand weapon they are holding
	ullong l_weapon_address;
	unsigned int l_weapon_id;
	//hurtbox size(range) of weapon. Bows/Magic have high range
	float weaponRange;
	//minimum distance from which bot will attack
	//an attempt to mitigate bot from being parried or otherwise punished while using very long weaps
	float minimumRange;
	//hurtbox size of spell
	float spellRange;
    //encompases the various states of an animation
    unsigned int subanimation;
    //the current animation id
    ullong animationId_address;
    //secondary animation id. Used rarely
    ullong animationId2_address;
	//tertiary animation id. Only used for backstabs?
	ullong animationId3_address;
	unsigned char in_backstab;
	//animation timer.
    ullong animationTimer_address;
    //secondary animation timer. Used rarely
    ullong animationTimer2_address;
	//hurtbox state 
    ullong hurtboxActive_address;
    //ready/animation switchable state
    ullong readyState_address;
    //velocity. used for backstab detection
    ullong velocity_address;
    float velocity;
    //if locked on. used for verification for counter strafe
    ullong locked_on_address;
    unsigned char locked_on;
    //time left before enemy hurtbox activates. Used for reverse roll vs dodge roll check
    float dodgeTimeRemaining;
    //if player is two handing or not
    ullong twoHanding_address;
    unsigned char twoHanding;
    //stamina recovery rate
    ullong staminaRecoveryRate_address;
    int staminaRecoveryRate;
    //current poise
    ullong poise_address;
    float poise;
    //current bleed state
    ullong bleedStatus_address;
    int bleedStatus;
	//check weapon type for attacks
	int WeaponRoutines;
	//Attempt to influence weight of blocking to rolling for builds with good shields, when shield is out & not at low hp/stam
	int DefendRoutines;
	//If a spell tool is equipped in either hand
	int isSpellTool;
} Character;

int EnemyWeaponClass;

//initalize the phantom and player
Character Enemy;
Character Player;
#define EnemyId 0
#define PlayerId 1

//read memory for the character's variables
void ReadPlayer(Character * c, HANDLE processHandle, int characterId);

void ReadPlayerDEBUGGING(Character * c, HANDLE processHandle, ...);

void ReadPointerEndAddresses(HANDLE processHandle);

//TODO prune as many of these as possible. what needs to be kept for only one char?

//basic values and offsets we use
//the base address, which offsets are added to
//this MUST be 64 bits to account for max possible address space
extern ullong Enemy_base_add;
extern ullong player_base_add;
//offsets and length for x location
static const int Enemy_loc_x_offsets[] = { 0x4, 0x4, 0x2C, 0x260 };
static const int Player_loc_x_offsets[] = { 0x3C, 0x330, 0x4, 0x20C, 0x3C0 };
static const int Enemy_loc_x_offsets_length = 4;
static const int Player_loc_x_offsets_length = 5;
//offsets and length for y location
static const int Enemy_loc_y_offsets[] = { 0x4, 0x4, 0x28, 0x54, 0x268 };
static const int Player_loc_y_offsets[] = { 0x3C, 0x330, 0x4, 0x20C, 0x3C8 };
static const int Enemy_loc_y_offsets_length = 5;
static const int Player_loc_y_offsets_length = 5;
//offsets and length for rotation.
static const int Enemy_rotation_offsets[] = { 0x4, 0x4, 0x28, 0x54, 0x34 };
static const int Player_rotation_offsets[] = { 0x3C, 0x28, 0x1C, 0x4 };
static const int Enemy_rotation_offsets_length = 5;
static const int Player_rotation_offsets_length = 4;
//offsets and length for animation type id
static const int Enemy_animationType_offsets[] = { 0x4, 0x4, 0x28, 0x54, 0x1EC };
static const int Player_animationType_offsets[] = { 0x288, 0xC, 0xC, 0x10, 0x41C };
static const int Enemy_animationType_offsets_length = 5;
static const int Player_animationType_offsets_length = 5;
//offsets and length for PassiveState id
static const int Enemy_passiveState_offsets[] = { 0x3C, 0x0C, 0x20, 0x28, 0x54, 0x1F0 };
static const int Player_passiveState_offsets[] = { 0x288, 0xC, 0xC, 0x10, 0x420 };
static const int Enemy_passiveState_offsets_length = 6;
static const int Player_passiveState_offsets_length = 5;
//hp
static const int Enemy_hp_offsets[] = { 0x4, 0x4, 0x2D4 };
static const int Player_hp_offsets[] = { 0x288, 0xC, 0x330, 0x4, 0x2D4 };
static const int Enemy_hp_offsets_length = 3;
static const int Player_hp_offsets_length = 5;
//stamina
static const int Player_stamina_offsets[] = { 0x288, 0xC, 0x330, 0x4, 0x2E4 };
static const int Player_stamina_offsets_length = 5;
//R weapon id
static const int Enemy_r_weapon_offsets[] = { 0x4, 0x4, 0x34C, 0x654, 0x1F8 };
static const int Player_r_weapon_offsets[] = { 0x3C, 0x30, 0xC, 0x654, 0x1F8 };
static const int Enemy_r_weapon_offsets_length = 5;
static const int Player_r_weapon_offsets_length = 5;
//L weapon id
static const int Enemy_l_weapon_offsets[] = { 0x4, 0x4, 0x34C, 0x654, 0x1B8 };
static const int Player_l_weapon_offsets[] = { 0x3C, 0x30, 0xC, 0x654, 0x1B4 };
static const int Enemy_l_weapon_offsets_length = 5;
static const int Player_l_weapon_offsets_length = 5;
//the current subanimation being executed
#define AttackSubanimationWindup 00
#define AttackSubanimationWindupClosing 01
#define AttackSubanimationWindupGhostHit 02
#define AttackSubanimationActiveDuringHurtbox 11
#define LockInSubanimation 12
#define AttackSubanimationActiveHurtboxOver 13
#define PoiseBrokenSubanimation 14
#define SubanimationRecover 20
#define SubanimationNeutral 30
//if enemy's weapon's hurtbox is active
static const int Enemy_hurtboxActive_offsets[] = { 0x4, 0x0, 0xC, 0x3C, 0xF };
static const int Enemy_hurtboxActive_offsets_length = 5;
//time animation has been active
static const int Enemy_animationTimer_offsets[] = { 0x4, 0x4, 0x28, 0x18, 0x4DC };
static const int Enemy_animationTimer_offsets_length = 5;
static const int Player_animationTimer_offsets[] = { 0x28, 0x0, 0x148, 0x4C8, 0x4DC };
static const int Player_animationTimer_offsets_length = 5;
//second timer for animation. Note sometimes due to lag this will cut itself off early to that timer 1 can start at correct time
static const int Enemy_animationTimer2_offsets[] = { 0x4, 0x4, 0x28, 0x18, 0x440 };
static const int Enemy_animationTimer2_offsets_length = 5;
static const int Player_animationTimer2_offsets[] = { 0x28, 0x0, 0x148, 0x4C8, 0x440 };
static const int Player_animationTimer2_offsets_length = 5;
//current animation id
static const int Enemy_animationID_offsets[] = { 0x4, 0x4, 0x28, 0x18, 0x444 };
static const int Enemy_animationID_offsets_length = 5;
static const int Player_animationID_offsets[] = { 0x288, 0xC, 0x618, 0x28, 0x7B0 };
static const int Player_animationID_offsets_length = 5;
//second animation id
static const int Enemy_animationID2_offsets[] = { 0x4, 0x4, 0x28, 0x18, 0x3A8 };
static const int Enemy_animationID2_offsets_length = 5;
static const int Player_animationID2_offsets[] = { 0x3C, 0x28, 0x18, 0x8C, 0x1D4 };
static const int Player_animationID2_offsets_length = 5;
//teriary animation id
static const int Enemy_animationID3_offsets[] = { 0x4, 0x4, 0x65C, 0x268, 0x770 };
static const int Enemy_animationID3_offsets_length = 5;
static const int Player_animationID3_offsets[] = { 0x3C, 0x10C };
static const int Player_animationID3_offsets_length = 2;
//if in a ready/animation switchable state
static const int Player_readyState_offsets[] = { 0x3C, 0x30, 0xC, 0x20C, 0x7D2 };
static const int Player_readyState_offsets_length = 5;
//speed the opponent is approaching at. Player doesnt need to know their own. Idealy would like just if sprinting or not, actual velocity isnt important
//-0.04 slow walk
//-0.13 walk
//-0.16 - 18 sprint
static const int Enemy_velocity_offsets[] = { 0x4, 0x4, 0x658, 0x5C, 0x3BC };
static const int Enemy_velocity_offsets_length = 5;
//if player is locked on. used for verification only
static const int Player_Lock_on_offsets[] = { 0x3C, 0x170, 0x2C, 0x390, 0x128 };
static const int Player_Lock_on_offsets_length = 5;
//handed state of player
static const int Player_twohanding_offsets[] = { 0x28, 0x0, 0x148, 0x4C8, 0x0 };
static const int Player_twohanding_offsets_length = 5;
//stamina recovery rate of enemy
static const int Enemy_stamRecovery_offsets[] = { 0x4, 0x4, 0x170, 0x34C, 0x408 };
static const int Enemy_stamRecovery_offsets_length = 5;
//current poise
static const int Player_Poise_offsets[] = { 0x28, 0x18, 0xE0, 0xC, 0x1C0 };
static const int Player_Poise_offsets_length = 5;
static const int Enemy_Poise_offsets[] = { 0x4, 0x4, 0x60, 0x8, 0x1C0 };
static const int Enemy_Poise_offsets_length = 5;
//bleed status
static const int Player_BleedStatus_offsets[] = { 0x3C, 0x308 };
static const int Player_BleedStatus_offsets_length = 2;
//spell slots I AM PROBABLY DOING THIS POORLY
static const int Player_Spell_1_offsets[] = { 0x20, 0x20, 0x20, 0x20, 0x0, 0xD7C };
static const int Player_Spell_1_offsets_length = 6;
static const int Player_Casts_1_offset[] = { 0x20, 0x20, 0x20, 0x20, 0x0, 0xD80 };
static const int Player_Casts_1_offsets_length = 6;
static const int Player_Spell_2_offsets[] = { 0x20, 0x20, 0x20, 0x20, 0x0, 0xD84 };
static const int Player_Spell_2_offsets_length = 6;
static const int Player_Casts_2_offset[] = { 0x20, 0x20, 0x20, 0x20, 0x0, 0xD88 };
static const int Player_Casts_2_offsets_length = 6;
static const int Player_Spell_3_offsets[] = { 0x20, 0x20, 0x20, 0x20, 0x0, 0xD8C };
static const int Player_Spell_3_offsets_length = 6;
static const int Player_Casts_3_offset[] = { 0x20, 0x20, 0x20, 0x20, 0x0, 0xD90 };
static const int Player_Casts_3_offsets_length = 6;
static const int Player_Spell_4_offsets[] = { 0x20, 0x20, 0x20, 0x20, 0x0, 0xD94 };
static const int Player_Spell_4_offsets_length = 6;
static const int Player_Casts_4_offset[] = { 0x20, 0x20, 0x20, 0x20, 0x0, 0xD98 };
static const int Player_Casts_4_offsets_length = 6;
static const int Player_Spell_5_offsets[] = { 0x20, 0x20, 0x20, 0x20, 0x0, 0xD9C };
static const int Player_Spell_5_offsets_length = 6;
static const int Player_Casts_5_offset[] = { 0x20, 0x20, 0x20, 0x20, 0x0, 0xDA0 };
static const int Player_Casts_5_offsets_length = 6;
static const int Player_Spell_6_offsets[] = { 0x20, 0x20, 0x20, 0x20, 0x0, 0xDA4 };
static const int Player_Spell_6_offsets_length = 6;
static const int Player_Casts_6_offset[] = { 0x20, 0x20, 0x20, 0x20, 0x0, 0xDA8 };
static const int Player_Casts_6_offsets_length = 6;
static const int Player_Spell_7_offsets[] = { 0x20, 0x20, 0x20, 0x20, 0x0, 0xDAC };
static const int Player_Spell_7_offsets_length = 6;
static const int Player_Casts_7_offset[] = { 0x20, 0x20, 0x20, 0x20, 0x0, 0xDB0 };
static const int Player_Casts_7_offsets_length = 6;
static const int Player_Spell_8_offsets[] = { 0x20, 0x20, 0x20, 0x20, 0x0, 0xDB4 };
static const int Player_Spell_8_offsets_length = 6;
static const int Player_Casts_8_offset[] = { 0x20, 0x20, 0x20, 0x20, 0x0, 0xDB8 };
static const int Player_Casts_8_offsets_length = 6;
static const int Player_Spell_9_offsets[] = { 0x20, 0x20, 0x20, 0x20, 0x0, 0xDBC };
static const int Player_Spell_9_offsets_length = 6;
static const int Player_Casts_9_offset[] = { 0x20, 0x20, 0x20, 0x20, 0x0, 0xDC0 };
static const int Player_Casts_9_offsets_length = 6;
static const int Player_Spell_10_offsets[] = { 0x20, 0x20, 0x20, 0x20, 0x0, 0xDC4 };
static const int Player_Spell_10_offsets_length = 6;
static const int Player_Casts_10_offset[] = { 0x20, 0x20, 0x20, 0x20, 0x0, 0xDC8 };
static const int Player_Casts_10_offsets_length = 6;
static const int Player_Spell_11_offsets[] = { 0x20, 0x20, 0x20, 0x20, 0x0, 0xDCC };
static const int Player_Spell_11_offsets_length = 6;
static const int Player_Casts_11_offset[] = { 0x20, 0x20, 0x20, 0x20, 0x0, 0xDD0 };
static const int Player_Casts_11_offsets_length = 6;
static const int Player_Spell_12_offsets[] = { 0x20, 0x20, 0x20, 0x20, 0x0, 0xDD4 };
static const int Player_Spell_12_offsets_length = 6;
static const int Player_Casts_12_offset[] = { 0x20, 0x20, 0x20, 0x20, 0x0, 0xDD8 };
static const int Player_Casts_12_offsets_length = 6;

#endif