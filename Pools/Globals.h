#pragma once

#include "UtilitiesConfig.h"
#include "../../inc/natives.h"

struct HitPacket
{
    bool valid = false;

    int* event{};

    int damage = 0;
    int startHealth = 0;
    int endHealth = 0;

    float damagePercentFullHealth = 0.0f;
    float damagePercentCurrentHealth = 0.0f;

    Hash equippedWeapon;

    Ped attacker;
    Hash attackerWeapon;
    Vector3 attackerDirection;

    int bone = -1;
    bool hasBone = false;

    int ragdollType = -1;
    int ragdollDuration = 0;
    float forceIntensity = 0.0f;
};

const Hash hash_EVENT_ENTITY_DAMAGED =
GAMEPLAY::GET_HASH_KEY("EVENT_ENTITY_DAMAGED");

Ped player = 0;
int lastHealth = 0;
HitPacket hitPacket;

int captureFramesForBoneProcessing = 0;
int framesToAttemptForceEffect = 0;

int ragdollCooldownSetting = 0;
int forceEffectCooldownSetting = 0;

int ragdollCooldown = 0;
int forceEffectCooldown = 0;

int radollDurationForType0 = 0;
int radollDurationForType1 = 0;
int radollDurationForType2 = 0;
int radollDurationForType3 = 0;

float healthToCauseType0 = 0.0f;
float healthToCauseType1 = 0.0f;
float healthToCauseType2 = 0.0f;
float healthToCauseType3 = 0.0f;

float forceBaseToApplyToRagdoll = 0.0f;

int chanceToRegisterHitSetting = 100;

bool useTotalHealthForPercentSetting = false;

bool immunityRevolver = false;
bool immunityPistol = false;
bool immunityRepeater = false;
bool immunityRifle = false;
bool immunityShotgun = false;
bool immunitySniper = false;
bool immunityBow = false;
bool immunityMelee = false;

inline void SetConfigs()
{
    ragdollCooldownSetting =
        static_cast<int>(GetIniFloat("ragdollCooldown") * 1000);

    forceEffectCooldownSetting =
        static_cast<int>(GetIniFloat("forceEffectCooldown") * 1000);

    radollDurationForType0 =
        static_cast<int>(GetIniFloat("radollDurationForType0") * 1000);

    radollDurationForType1 =
        static_cast<int>(GetIniFloat("radollDurationForType1") * 1000);

    radollDurationForType2 =
        static_cast<int>(GetIniFloat("radollDurationForType2") * 1000);

    radollDurationForType3 =
        static_cast<int>(GetIniFloat("radollDurationForType3") * 1000);

    healthToCauseType0 =
        GetIniFloat("healthToCauseType0") / 100.0f;

    healthToCauseType1 =
        GetIniFloat("healthToCauseType1") / 100.0f;

    healthToCauseType2 =
        GetIniFloat("healthToCauseType2") / 100.0f;

    healthToCauseType3 =
        GetIniFloat("healthToCauseType3") / 100.0f;

    forceBaseToApplyToRagdoll =
        GetIniFloat("forceBaseToApplyToRagdoll");

    chanceToRegisterHitSetting =
        static_cast<int>(GetIniFloat("chanceToRegisterHit"));

    useTotalHealthForPercentSetting =
        ConvertIntToBool(
            GetIniInt("useTotalHealthForPercent")
        );

    immunityRevolver =
        ConvertIntToBool(
            GetIniInt("immunityRevolver")
        );

    immunityPistol =
        ConvertIntToBool(
            GetIniInt("immunityPistol")
        );

    immunityRepeater =
        ConvertIntToBool(
            GetIniInt("immunityRepeater")
        );

    immunityRifle =
        ConvertIntToBool(
            GetIniInt("immunityRifle")
        );

    immunityShotgun =
        ConvertIntToBool(
            GetIniInt("immunityShotgun")
        );

    immunitySniper =
        ConvertIntToBool(
            GetIniInt("immunitySniper")
        );

    immunityBow =
        ConvertIntToBool(
            GetIniInt("immunityBow")
        );

    immunityMelee =
        ConvertIntToBool(
            GetIniInt("immunityMelee")
        );
}