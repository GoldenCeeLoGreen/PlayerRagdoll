#pragma once

#include "Globals.h"
#include "UtilitiesChecks.h"
#include "UtilitiesConfig.h"
#include "UtilitiesGets.h"
#include "UtilitiesLogger.h"
#include "UtilitiesRandom.h"


//#include "..\..\inc\natives.h"

#include <cmath>
//#include <fstream>
//#include <sstream>

bool CaptureBoneData()
{
    int bone;
    Log("Collecting Bone Data");
    if (PED::GET_PED_LAST_DAMAGE_BONE(player, &bone))
    {
        Log("Bone Data Collected: " + ToString(bone));
        hitPacket.bone = bone;
        hitPacket.hasBone = true;
        return hitPacket.hasBone;
    }
    return false;
}

Vector3 ComputeDirection(Ped player, Ped attacker)
{
    Vector3 posPlayer = GetPedLocation(player);
    Vector3 posAttacker = GetPedLocation(attacker);

    Vector3 dir;
    dir.x = posPlayer.x - posAttacker.x;
    dir.y = posPlayer.y - posAttacker.y;
    dir.z = posPlayer.z - posAttacker.z;

	Log("Player Position: " + ToString(posPlayer.x) + ", " + ToString(posPlayer.y) + ", " + ToString(posPlayer.z));
	Log("Attacker Position: " + ToString(posAttacker.x) + ", " + ToString(posAttacker.y) + ", " + ToString(posAttacker.z));

    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);

	Log("Direction Vector: " + ToString(dir.x) + ", " + ToString(dir.y) + ", " + ToString(dir.z));
	Log("Direction Length: " + ToString(len));

    if (len == 0.0f)
		len = 1.0f; // Prevent division by zero prolly a bug

    dir.x /= len;
    dir.y /= len;
    dir.z /= len;

	Log("Normalized Direction Vector: " + ToString(dir.x) + ", " + ToString(dir.y) + ", " + ToString(dir.z));

    return dir;
}

int GetReactionFromDamage(float percent)
{
    Log("Determining reaction type for damage percent: " + ToString(percent));
    if (percent > healthToCauseType0)
        return 0;
    if (percent > healthToCauseType1)
        return 1;
    if (percent > healthToCauseType2)
        return 2;
    if (percent > healthToCauseType3)
        return 3;
    return -1;
}

void ApplyRagdoll(Ped player, int duration, int ragdolltype)
{
	Log("Player Ragdolled for " + ToString(duration) + "ms with type " + ToString(ragdolltype));
    PED::SET_PED_TO_RAGDOLL(player, duration, duration, ragdolltype, true, true, false);
}

float GetDamagePercentFromHitPacket(bool FullHealth)
{
    if (FullHealth)
        return hitPacket.damagePercentFullHealth;
    else
        return hitPacket.damagePercentCurrentHealth;
}

int DetermineRagdollType(Ped player)
{
    int reaction = GetReactionFromDamage(GetDamagePercentFromHitPacket(useTotalHealthForPercentSetting));
    return reaction;
}

bool DetermineShouldApplyRagdoll()
{
    //if (Random1To100() > chanceToRegisterHitSetting)
    //    return false


    if (CheckWeaponIsRevolver(hitPacket.equippedWeapon) && immunityRevolver)
        return false;
    if (CheckWeaponIsPistol(hitPacket.equippedWeapon) && immunityPistol)
        return false;
    if (CheckWeaponIsRepeater(hitPacket.equippedWeapon) && immunityRepeater)
        return false;
    if (CheckWeaponIsRifle(hitPacket.equippedWeapon) && immunityRifle)
        return false;
    if (CheckWeaponIsShotgun(hitPacket.equippedWeapon) && immunityShotgun)
        return false;
    if (CheckWeaponIsSniper(hitPacket.equippedWeapon) && immunitySniper)
        return false;
    if (CheckWeaponIsBow(hitPacket.equippedWeapon) && immunityBow)
        return false;
    if (CheckWeaponIsMelee(hitPacket.equippedWeapon) && immunityMelee)
        return false;

    return true;
}

void ClearHitPacket()
{
    Log("Clearing Hit Packet");
    Log("_____________________");
    hitPacket = HitPacket();
}

int DetermineDuration(int ragdollType)
{
    switch (ragdollType)
    {
        case 0:
            return radollDurationForType0;
        case 1:
            return radollDurationForType1;
        case 2:
            return radollDurationForType2;
        case 3:
            return radollDurationForType3;
        default:
            return 0;
    }
}

void ApplyHitReaction()
{
    bool appliedForce = false;

    Log("Ragdoll Cooldown Duration " + ToString(ragdollCooldown));
    if (ragdollCooldown == 0) //This should be a "Should do ragdoll" function
    {
        if (hitPacket.ragdollType >= 0 && !PED::_0x3BDFCF25B58B0415(player))
        {
            ApplyRagdoll(player, hitPacket.ragdollDuration, hitPacket.ragdollType);
            framesToAttemptForceEffect = 5;
            ragdollCooldown = ragdollCooldownSetting;
        }
    }

    Log("Force Cooldown Duration " + ToString(forceEffectCooldown));
    if (forceEffectCooldown == 0) //This should be a "Should do force" function
    {
        Log("Force applied");
        ENTITY::APPLY_FORCE_TO_ENTITY(
            player,
            1,
            hitPacket.attackerDirection.x * hitPacket.forceIntensity,
            hitPacket.attackerDirection.y * hitPacket.forceIntensity,
            hitPacket.attackerDirection.z * hitPacket.forceIntensity,
            0.0f, 0.0f, 0.0f,
            hitPacket.bone,
            true,
            true,
            true,
            false,
            false
        );
        appliedForce = true;
        forceEffectCooldown = forceEffectCooldownSetting;
    }

    if ( appliedForce || framesToAttemptForceEffect == 0 )
    {
        ClearHitPacket();
    }
}

void UpdateHitSystem()
{
	Log("Updating Hit System");
    if(captureFramesForBoneProcessing == 0)
    {
        captureFramesForBoneProcessing = 10;

		hitPacket.damage = hitPacket.event[4];
		Log("Event Damage Logged: " + ToString(hitPacket.damage));
		hitPacket.startHealth = lastHealth;
        Log("Event startHealth Logged: " + ToString(hitPacket.startHealth));
        hitPacket.endHealth = hitPacket.startHealth - hitPacket.damage;
        Log("Event endHealth Logged: " + ToString(hitPacket.startHealth));
        hitPacket.equippedWeapon = GetPedWeapon(player);
		Log("Event equippedWeapon Logged: " + ToString(hitPacket.equippedWeapon));
		hitPacket.ragdollType = DetermineRagdollType(player);
		Log("Event ragdollType Logged: " + ToString(hitPacket.ragdollType));
		hitPacket.ragdollDuration = DetermineDuration(hitPacket.ragdollType);
		Log("Event ragdollDuration Logged: " + ToString(hitPacket.ragdollDuration));
        hitPacket.attacker = hitPacket.event[1];
		Log("Event attacker Logged: " + ToString(hitPacket.attacker));
		hitPacket.attackerDirection = ComputeDirection(player, hitPacket.attacker);
		Log("Event direction Logged: " + ToString(hitPacket.attackerDirection.x) + ", " + ToString(hitPacket.attackerDirection.y) + ", " + ToString(hitPacket.attackerDirection.z));
		hitPacket.attackerWeapon = GetPedWeapon(hitPacket.attacker);
		Log("Event attackerWeapon Logged: " + ToString(hitPacket.attackerWeapon));
		hitPacket.damagePercentFullHealth = (float)hitPacket.damage / (float)ENTITY::GET_ENTITY_MAX_HEALTH(player, true);
		Log("Event damagePercentFullHealth Logged: " + ToString(hitPacket.damagePercentFullHealth));
		hitPacket.damagePercentCurrentHealth = (float)hitPacket.damage / (float)hitPacket.startHealth;
		Log("Event damagePercentCurrentHealth Logged: " + ToString(hitPacket.damagePercentCurrentHealth));
		hitPacket.ragdollType = DetermineRagdollType(player);
		Log("Event ragdollType Logged: " + ToString(hitPacket.ragdollType));
		hitPacket.ragdollDuration = DetermineDuration(hitPacket.ragdollType);
		Log("Event ragdollDuration Logged: " + ToString(hitPacket.ragdollDuration));

        // Make determine forceIntensity that includes a bunch of config
        hitPacket.forceIntensity = (GetDamagePercentFromHitPacket(useTotalHealthForPercentSetting) * forceBaseToApplyToRagdoll);
        Log("Event forceIntensity Logged: " + ToString(hitPacket.forceIntensity));

        //ADD ALL THE STUFF TO THE HIT PACKET. REMOVE STUFF FROM APPLY HIT REACTION AND SAVE IT HERE.
	}

    if (captureFramesForBoneProcessing > 0)
    {
		Log("Collecting Bone Data");
        if (CaptureBoneData())
        {
            Log("Hitpacketvalid");
            hitPacket.valid = true;
            captureFramesForBoneProcessing = 0;
            return;
        }
    }
    else
    {
        ClearHitPacket();
	}
}

void GetDamageEventData()
{
    int eventCount = SCRIPT::GET_NUMBER_OF_EVENTS(0);

    for (int i = 0; i < eventCount; ++i)
    {
        Hash eventType = SCRIPT::GET_EVENT_AT_INDEX(0, i);
        if (eventType != hash_EVENT_ENTITY_DAMAGED)
            continue;

        int* eventData{};
        if (!SCRIPT::GET_EVENT_DATA(0, i, eventData, 9))
            continue;

        if(eventData[0] != player)
			continue;

        Log("_____________________");
		Log("Damage Event Detected");
        
		hitPacket.event = eventData;
        return;
    }
}

void DecrementCooldowns()
{
    if (ragdollCooldown > 0) { ragdollCooldown--; }
    if (forceEffectCooldown > 0) { forceEffectCooldown--; }
    if (framesToAttemptForceEffect > 0) { framesToAttemptForceEffect--; }
	if (captureFramesForBoneProcessing > 0) { captureFramesForBoneProcessing--; }
}

void mainLoop()
{
    while (true)
    {
        player = PLAYER::PLAYER_PED_ID();
        DecrementCooldowns();
        if (CheckValidPlayer(player))
        {
            if (!hitPacket.event){GetDamageEventData();}
            if (hitPacket.event && !hitPacket.valid){UpdateHitSystem();}
            if (hitPacket.valid){ApplyHitReaction();}
        }
        lastHealth = ENTITY::GET_ENTITY_HEALTH(player);
        WAIT(0);
    }
}

void ScriptMain()
{
    InitializeLogFile();
    SetConfigs();

    Ped player = PLAYER::PLAYER_PED_ID();
    lastHealth = ENTITY::GET_ENTITY_HEALTH(player);

    mainLoop();
}