#pragma once

#include "../../inc/natives.h"
#include "../../inc/types.h"
#include "../../inc/main.h"
#include "../../inc/nativeCaller.h"
#include "../../inc/enums.h"
#include "../../inc/UtilitiesLogger.h"
#include "../../inc/UtilitiesGets.h"
#include "../../inc/UtilitiesConfig.h"
#include "../../inc/UtilitiesChecks.h"
#include "../../inc/UtilitiesRandom.h"
#include "../../inc/UtilitiesMisc.h"

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

// Values match the ragdollType argument of SET_PED_TO_RAGDOLL (type 2 is rejected by the game, so it is skipped)
enum class RagdollType : int
{
    None = -1,
    Normal = 0,
    StiffFall = 1,
    WideStumble = 3
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

    Hash equippedWeapon = 0;

    Ped attacker = 0;
    Hash attackerWeapon = 0;
    Vector3 attackerDirection{};

    RagdollType ragdollType = RagdollType::None;
    int ragdollDuration = 0;
    float forceIntensity = 0.0f;

    bool rollPassed = false;
};

// State
Ped player = 0;
int lastHealth = 0;
HitPacket hitPacket;

// Game time (ms) each effect was last applied; 0 = never
int timeLastRagdoll = 0;
int timeLastForceEffect = 0;

// Config (settingDebugLogger lives in UtilitiesLogger.h)
float settingCooldownRagdoll = 0;
float settingCooldownForceEffect = 0;

bool settingIgnoreHitsWhileRagdolled = true;
bool settingRequireHumanDamageSource = false;
bool settingRequireGunDamageSource = false;

float settingHealthPercentToCauseType0 = 0.0f;
float settingHealthPercentToCauseType1 = 0.0f;
float settingHealthPercentToCauseType3 = 0.0f;

int settingRagdollDurationForType0 = 0;
int settingRagdollDurationForType1 = 0;
int settingRagdollDurationForType3 = 0;

float settingForceBaseToApplyToRagdoll = 0.0f;

bool settingUseTotalHealthForPercentSetting = false;

int settingChanceToApplyEffects = 100;

bool settingMeanShotguns = false;

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

bool settingImmunityHorse = false;
bool settingImmunityVehicle = false;
bool settingImmunityGrapple = false;
bool settingImmunityMeleeCombat = false;
bool settingImmunityDeadEye = false;
bool settingImmunityCarrying = false;

void SetWeaponImmunitySettings()
{
    settingImmunityRevolver = GetIniBool("settingImmunityRevolver");
    settingImmunityPistol = GetIniBool("settingImmunityPistol");
    settingImmunityRepeater = GetIniBool("settingImmunityRepeater");
    settingImmunityRifle = GetIniBool("settingImmunityRifle");
    settingImmunityShotgun = GetIniBool("settingImmunityShotgun");
    settingImmunitySniper = GetIniBool("settingImmunitySniper");
    settingImmunityBow = GetIniBool("settingImmunityBow");
    settingImmunityMelee = GetIniBool("settingImmunityMelee");
    settingImmunityLasso = GetIniBool("settingImmunityLasso");
    settingImmunityBinoculars = GetIniBool("settingImmunityBinoculars");
    settingImmunityLantern = GetIniBool("settingImmunityLantern");
    settingImmunityTorch = GetIniBool("settingImmunityTorch");
    settingImmunityKnife = GetIniBool("settingImmunityKnife");
}

void SetRagdollSettings()
{
    // Cooldowns and durations are entered in seconds in the ini
    settingCooldownRagdoll = GetIniFloat("settingCooldownRagdoll") * 1000;
    settingCooldownForceEffect = GetIniFloat("settingCooldownForceEffect") * 1000;

    settingIgnoreHitsWhileRagdolled = GetIniBool("settingIgnoreHitsWhileRagdolled");

    settingRagdollDurationForType0 = GetIniFloat("settingRagdollDurationForType0") * 1000;
    settingRagdollDurationForType1 = GetIniFloat("settingRagdollDurationForType1") * 1000;
    settingRagdollDurationForType3 = GetIniFloat("settingRagdollDurationForType3") * 1000;

    // Percent thresholds are entered as 0-100 in the ini
    settingHealthPercentToCauseType0 = GetIniFloat("settingHealthPercentToCauseType0") / 100.0f;
    settingHealthPercentToCauseType1 = GetIniFloat("settingHealthPercentToCauseType1") / 100.0f;
    settingHealthPercentToCauseType3 = GetIniFloat("settingHealthPercentToCauseType3") / 100.0f;

    settingForceBaseToApplyToRagdoll = GetIniFloat("settingForceBaseToApplyToRagdoll");
}

void SetConfigs()
{
    settingDebugLogger = GetIniBool("settingDebugLogger");

    settingChanceToApplyEffects = GetIniFloat("settingChanceToApplyEffects");
    settingUseTotalHealthForPercentSetting = GetIniBool("settingUseTotalHealthForPercentSetting");

    settingRequireHumanDamageSource = GetIniBool("settingRequireHumanDamageSource");
    settingRequireGunDamageSource = GetIniBool("settingRequireGunDamageSource");

    settingImmunityHorse = GetIniBool("settingImmunityHorse");
    settingImmunityVehicle = GetIniBool("settingImmunityVehicle");
    settingImmunityGrapple = GetIniBool("settingImmunityGrapple");
    settingImmunityMeleeCombat = GetIniBool("settingImmunityMeleeCombat");
    settingImmunityDeadEye = GetIniBool("settingImmunityDeadEye");
    settingImmunityCarrying = GetIniBool("settingImmunityCarrying");

    settingMeanShotguns = GetIniBool("settingMeanShotguns");

    SetRagdollSettings();
    SetWeaponImmunitySettings();
}
