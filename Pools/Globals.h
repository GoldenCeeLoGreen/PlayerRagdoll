#pragma once

#include "UtilitiesConfig.h"
#include "../../inc/natives.h"
#include "UtilitiesLogger.h"

struct EventEntityDamaged
{
    alignas(8) Entity target;
    alignas(8) Entity source;
    alignas(8) Hash weapon;
    alignas(8) Hash ammo;
    alignas(8) float damage;
    alignas(8) int unknown;
    alignas(8) float x;
    alignas(8) float y;
    alignas(8) float z;
};

struct HitPacket
{
    bool valid = false;
	bool validEvent = false;

    EventEntityDamaged event{};

    float damage = 0;
    float fullHealth = 0;
    float startHealth = 0;

    float damagePercentFullHealth = 0.0f;
    float damagePercentCurrentHealth = 0.0f;

    Hash equippedWeapon;

    Ped attacker;
    Hash attackerWeapon;
    Vector3 attackerDirection;

    bool bulkInfoAdded = false;

    int bone = -1;
    bool hasBone = false;

    int ragdollType = -1;
    int ragdollDuration = 0;
    float forceIntensity = 0.0f;

	bool ragdollApplied = false;
	bool forceApplied = false;
	bool rollPassed = false;
};

Ped player = 0;
int lastHealth = 0;
HitPacket hitPacket;

//vars
int framesToAttemptBoneProcessing = 0;
int framesToAttemptForceEffect = 0;

int cooldownRagdoll = 0;
int cooldownForceEffect = 0;

//config vars
bool settingDebugLogger = false;

float settingCooldownRagdoll = 0;
float settingCooldownForceEffect = 0;

bool settingApplyRagdollIfRagdolled = false;

float settingHealthPercentToCauseType0 = 0.0f;
float settingHealthPercentToCauseType1 = 0.0f;
float settingHealthPercentToCauseType2 = 0.0f;
float settingHealthPercentToCauseType3 = 0.0f;

int settingRadollDurationForType0 = 0;
int settingRadollDurationForType1 = 0;
int settingRadollDurationForType2 = 0;
int settingRadollDurationForType3 = 0;

float settingForceBaseToApplyToRagdoll = 0.0f;

bool settingUseTotalHealthForPercentSetting = false;

int settingChanceToApplyEffects = 100;

bool settingImmunityRevolver = false;
bool settingImmunityPistol = false;
bool settingImmunityRepeater = false;
bool settingImmunityRifle = false;
bool settingImmunityShotgun = false;
bool settingImmunitySniper = false;
bool settingImmunityBow = false;
bool settingImmunityMelee = false;
bool settingImmunityLasso = false;
bool settingImmunityBinoculars = false;
bool settingImmunityLantern = false;
bool settingImmunityTorch = false;
bool settingImmunityKnife = false;

void SetWeaponImmunitySettings()
{
    settingImmunityRevolver =
        ConvertIntToBool(
            GetIniInt("immunityRevolver")
        );

    settingImmunityPistol =
        ConvertIntToBool(
            GetIniInt("immunityPistol")
        );

    settingImmunityRepeater =
        ConvertIntToBool(
            GetIniInt("immunityRepeater")
        );

    settingImmunityRifle =
        ConvertIntToBool(
            GetIniInt("immunityRifle")
        );

    settingImmunityShotgun =
        ConvertIntToBool(
            GetIniInt("immunityShotgun")
        );

    settingImmunitySniper =
        ConvertIntToBool(
            GetIniInt("immunitySniper")
        );

    settingImmunityBow =
        ConvertIntToBool(
            GetIniInt("immunityBow")
        );

    settingImmunityMelee =
        ConvertIntToBool(
            GetIniInt("immunityMelee")
        );

    settingImmunityLasso =
        ConvertIntToBool(
            GetIniInt("immunityLasso")
        );

    settingImmunityBinoculars =
        ConvertIntToBool(
            GetIniInt("immunityBinoculars")
        );

    settingImmunityLantern =
        ConvertIntToBool(
            GetIniInt("immunityLantern")
        );

    settingImmunityTorch =
        ConvertIntToBool(
            GetIniInt("immunityTorch")
        );

    settingImmunityKnife =
        ConvertIntToBool(
            GetIniInt("immunityKnife")
        );
}

void SetRagdollSettings()
{
    settingCooldownRagdoll =
        GetIniFloat("cooldownRagdoll") * 1000;

    settingCooldownForceEffect =
        GetIniFloat("cooldownForceEffect") * 1000;

    settingApplyRagdollIfRagdolled = 
        ConvertIntToBool(
            GetIniInt("settingApplyRagdollIfRagdolled")
		);  

    settingRadollDurationForType0 =
        GetIniFloat("settingRadollDurationForType0") * 1000;

    settingRadollDurationForType1 =
        GetIniFloat("settingRadollDurationForType1") * 1000;

    settingRadollDurationForType2 =
        GetIniFloat("settingRadollDurationForType2") * 1000;

    settingRadollDurationForType3 =
        GetIniFloat("settingRadollDurationForType3") * 1000;

    settingHealthPercentToCauseType0 =
        GetIniFloat("settingHealthPercentToCauseType0") / 100.0f;

    settingHealthPercentToCauseType1 =
        GetIniFloat("settingHealthPercentToCauseType1") / 100.0f;

    settingHealthPercentToCauseType2 =
        GetIniFloat("settingHealthPercentToCauseType2") / 100.0f;

    settingHealthPercentToCauseType3 =
        GetIniFloat("settingHealthPercentToCauseType3") / 100.0f;

    settingForceBaseToApplyToRagdoll =
        GetIniFloat("settingForceBaseToApplyToRagdoll");
}

void SetConfigs()
{
    settingDebugLogger = 
        ConvertIntToBool(
            GetIniInt("settingDebugLogger")
	    );

    settingChanceToApplyEffects =
        GetIniFloat("settingChanceToApplyEffects");

    settingUseTotalHealthForPercentSetting =
        ConvertIntToBool(
            GetIniInt("settingUseTotalHealthForPercentSetting")
        );

    SetRagdollSettings();

    SetWeaponImmunitySettings();
}