#include "script.h"

#include "Globals.h"

#include "UtilitiesChecks.h"
#include "UtilitiesConfig.h"
#include "UtilitiesGets.h"
#include "UtilitiesLogger.h"
#include "UtilitiesRandom.h"

#include <iomanip>
#include <cstdint>

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

void ClearHitPacket()
{
    Log("Clearing Hit Packet");
    Log("_____________________");
    hitPacket = HitPacket();
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
    if (percent > settingHealthPercentToCauseType0)
        return 0;
    if (percent > settingHealthPercentToCauseType1)
        return 1;
    if (percent > settingHealthPercentToCauseType2)
        return 2;
    if (percent > settingHealthPercentToCauseType3)
        return 3;
    return -1;
}

void ApplyRagdoll(Ped player, int duration, int ragdolltype)
{
	Log("Player Ragdolled for " + ToString(duration) + "ms with type " + ToString(ragdolltype));
    PED::SET_PED_TO_RAGDOLL(player, duration, duration, ragdolltype, true, true, false);
}

void ApplyForce(Ped ped, Vector3 direction, float intensity, int bone)
{
    Log("Force applied");
    ENTITY::APPLY_FORCE_TO_ENTITY(
        ped,
        1,
        direction.x * intensity,
        direction.y * intensity,
        direction.z * intensity,
        0.0f, 0.0f, 0.0f,
        bone,
        true,
        true,
        true,
        false,
        false
    );
}

float GetDamagePercentFromHitPacket()
{
    if (settingUseTotalHealthForPercentSetting)
        return hitPacket.damagePercentFullHealth;
    else
        return hitPacket.damagePercentCurrentHealth;
}

int DetermineRagdollType()
{
    int reaction = GetReactionFromDamage(GetDamagePercentFromHitPacket());
    return reaction;
}

bool CheckImmuneWeapon()
{
    if (settingImmunityRevolver) {
        if (CheckWeaponIsRevolver(hitPacket.equippedWeapon))
        {
            Log("Revolver is immune to effects");
            return true;
        }
    }
    if (settingImmunityPistol) {
        if (CheckWeaponIsPistol(hitPacket.equippedWeapon))
        {
            Log("Pistol is immune to effects");
            return true;
        }
    }
    if (settingImmunityRepeater) {
        if (CheckWeaponIsRepeater(hitPacket.equippedWeapon))
        {
            Log("Repeater is immune to effects");
            return true;
        }
    }
    if (settingImmunityRifle) {
        if (CheckWeaponIsRifle(hitPacket.equippedWeapon))
        {
            Log("Rifle is immune to effects");
            return true;
        }
    }
    if (settingImmunityShotgun) {
        if (CheckWeaponIsShotgun(hitPacket.equippedWeapon))
        {
            Log("Shotgun is immune to effects");
            return true;
        }
    }
    if (settingImmunitySniper) {
        if (CheckWeaponIsSniper(hitPacket.equippedWeapon))
        {
            Log("Sniper is immune to effects");
			return true;    
        }
    }
    if (settingImmunityBow) {
        if (CheckWeaponIsBow(hitPacket.equippedWeapon))
        {
            Log("Bow is immune to effects");
            return true;
        }
    }
    if (settingImmunityMelee) {
        if (CheckWeaponIsMelee(hitPacket.equippedWeapon))
        {
            Log("Melee is immune to effects");
            return true;
        }
    }

    return false;
}

bool DetermineShouldApplyRagdoll()
{
    if (hitPacket.ragdollApplied)
    {
        Log("Ragdoll has already been applied for this hit event");
        return false;
    }

    if (CheckImmuneWeapon())
    {
        Log("Weapon is immune to ragdoll effect");
        return false;
    }

    if (settingApplyRagdollIfRagdolled)
    {
        if (CheckIsPedRagdolled(player))
        {
            Log("Player is already ragdolled, and setting is preventing it being reapplied");
            return false;
        }
    }

    if(cooldownRagdoll > 0)
    {
        Log("Ragdoll is on cooldown for " + ToString(cooldownRagdoll) + " more frames");
        return false;
	}

	Log("Ragdoll should be applied");
    return true;
}

bool DetermineShouldForceEffect()
{
    if(CheckImmuneWeapon())
    {
        Log("Weapon is immune to force effect");
        return false;
	}

    if (!hitPacket.ragdollApplied)
    {
        Log("Ragdoll has not been applied for this hit event");
        return false;
    }

    if (cooldownForceEffect > 0)
    {
        Log("Force Effect is on cooldown for " + ToString(cooldownForceEffect) + " more frames");
        return false;
    }

    if(!(hitPacket.ragdollType >= 0) && !(hitPacket.ragdollType <= 4))
    {
        Log("Ragdoll type is invalid, cannot apply force effect");
        return false;
	}

	Log("Force effect should be applied");
    return true;
}

int DetermineDuration(int ragdollType)
{
    switch (ragdollType)
    {
        case 0:
            return settingRadollDurationForType0;
        case 1:
            return settingRadollDurationForType1;
        case 2:
            return settingRadollDurationForType2;
        case 3:
            return settingRadollDurationForType3;
        default:
            return 0;
    }
}

float DetermineForceIntensity()
{
    float intensityBase = settingForceBaseToApplyToRagdoll;
	Log("Intensity base: " + ToString(intensityBase));
	float scaledPercent = ((GetDamagePercentFromHitPacket() / 100.0f) * 10.0f) + 90.0f;
	Log("Scaled percent: " + ToString(scaledPercent) + "Based on damage percent" + ToString(GetDamagePercentFromHitPacket()));
	float finalIntensity = intensityBase * (scaledPercent / 100.0f);
	Log("Final intensity: " + ToString(finalIntensity));
    return finalIntensity;
}

bool DetermineShouldClearHitPacket()
{
    if (!hitPacket.rollPassed)
    {
        Log("Roll did not pass, clearing hit packet");
		return true;
    }

    if(hitPacket.forceApplied)
    {
        Log("Force has been applied, clearing hit packet");
        return true;
    }

    if (framesToAttemptForceEffect == 0 && hitPacket.ragdollApplied)
    {
        Log("Frames to attempt force effect has reached 0, clearing hit packet");
		return true;
    }

	Log("Hit packet should not be cleared yet");
    return false;
}

void ApplyHitReaction()
{
    if(DetermineShouldClearHitPacket())
    {
        ClearHitPacket();
        return;
	}

    Log("Ragdoll Cooldown Duration " + ToString(cooldownRagdoll));
    if (DetermineShouldApplyRagdoll())
    {

        ApplyRagdoll(player, hitPacket.ragdollDuration, hitPacket.ragdollType);
        framesToAttemptForceEffect = 30;
        cooldownRagdoll = settingCooldownRagdoll;
		hitPacket.ragdollApplied = true;
    }

	Log("framesToAttemptForceEffect " + ToString(framesToAttemptForceEffect));
    Log("Force Cooldown Duration " + ToString(cooldownForceEffect));
    if (DetermineShouldForceEffect())
    {
        ApplyForce(player, hitPacket.attackerDirection, hitPacket.forceIntensity, hitPacket.bone);
        hitPacket.forceApplied = true;
        cooldownForceEffect = settingCooldownForceEffect;
    }
}

void UpdateHitSystem()
{
	Log("Updating Hit System");
    if(!hitPacket.bulkInfoAdded)
    {
        framesToAttemptBoneProcessing = 10;

        hitPacket.equippedWeapon = GetPedWeapon(player);
        Log("Event equippedWeapon Logged: " + ToString(hitPacket.equippedWeapon));

		hitPacket.fullHealth = ENTITY::GET_ENTITY_MAX_HEALTH(player, true);
		Log("Event fullHealth Logged: " + ToString(hitPacket.fullHealth));
		hitPacket.damage = hitPacket.event.damage;
		Log("Event Damage Logged: " + ToString(hitPacket.damage));
		hitPacket.startHealth = lastHealth;
        Log("Event startHealth Logged: " + ToString(hitPacket.startHealth));
        hitPacket.damagePercentFullHealth = hitPacket.damage / hitPacket.fullHealth;
        Log("Event damagePercentFullHealth Logged: " + ToString(hitPacket.damagePercentFullHealth));
        hitPacket.damagePercentCurrentHealth = hitPacket.damage / hitPacket.startHealth;
        Log("Event damagePercentCurrentHealth Logged: " + ToString(hitPacket.damagePercentCurrentHealth));

		hitPacket.ragdollType = DetermineRagdollType();
		Log("Event ragdollType Logged: " + ToString(hitPacket.ragdollType));
		hitPacket.ragdollDuration = DetermineDuration(hitPacket.ragdollType);
		Log("Event ragdollDuration Logged: " + ToString(hitPacket.ragdollDuration));

        hitPacket.attacker = hitPacket.event.source;
		Log("Event attacker Logged: " + ToString(hitPacket.attacker));
        //Should I confirm an attacker exists?
		hitPacket.attackerDirection = ComputeDirection(player, hitPacket.attacker);
		Log("Event direction Logged: " + ToString(hitPacket.attackerDirection.x) + ", " + ToString(hitPacket.attackerDirection.y) + ", " + ToString(hitPacket.attackerDirection.z));
		hitPacket.attackerWeapon = GetPedWeapon(hitPacket.attacker);
		Log("Event attackerWeapon Logged: " + ToString(hitPacket.attackerWeapon));

        // Make determine forceIntensity that includes a bunch of config
        hitPacket.forceIntensity = DetermineForceIntensity();
        Log("Event forceIntensity Logged: " + ToString(hitPacket.forceIntensity));

        int roll = Random1To100();
        hitPacket.rollPassed = roll > settingChanceToApplyEffects;
		Log("Event roll Logged: " + ToString(roll) + " Roll Passed: " + ToString(hitPacket.rollPassed));

		hitPacket.bulkInfoAdded = true;
	}

    if (framesToAttemptBoneProcessing > 0)
    {
		Log("Collecting Bone Data");
        if (CaptureBoneData())
        {
            Log("Hitpacketvalid");
            hitPacket.valid = true;
            framesToAttemptBoneProcessing = 0;
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

        if (eventType != MISC::GET_HASH_KEY("EVENT_ENTITY_DAMAGED"))
            continue;

        EventEntityDamaged event{};

        if (!SCRIPT::GET_EVENT_DATA(0, i,(&event), 9))
            continue;
        
        if (event.target != player)
            continue;

        Log("_____________________");
        Log("Damage Event Detected");
		Log("Event Type: " + ToString(eventType));
        Log("Target: " + ToString(event.target));
        Log("Source: " + ToString(event.source));
        Log("Weapon: " + ToString(event.weapon));
        Log("Ammo: " + ToString(event.ammo));
        Log("Damage: " + ToString(event.damage));
        Log("Unknown: " + ToString(event.unknown));
        Log("Coords: " + ToString(event.x) + ", " + ToString(event.y) + ", " + ToString(event.z));

        hitPacket.event = event;
        hitPacket.validEvent = true;

        return;
    }
}

void DecrementCooldowns()
{
    if (cooldownRagdoll > 0) { cooldownRagdoll--; }
    if (cooldownForceEffect > 0) { cooldownForceEffect--; }
    if (framesToAttemptForceEffect > 0) { framesToAttemptForceEffect--; }
	if (framesToAttemptBoneProcessing > 0) { framesToAttemptBoneProcessing--; }
}

void mainLoop()
{
    while (true)
    {
        player = PLAYER::PLAYER_PED_ID();
        DecrementCooldowns();
        if (CheckValidPed(player))
        {
            if (!hitPacket.validEvent){GetDamageEventData();}
            if (hitPacket.validEvent && !hitPacket.valid){UpdateHitSystem();}
            if (hitPacket.valid){ApplyHitReaction();}
        }
        lastHealth = ENTITY::GET_ENTITY_HEALTH(player);
        WAIT(0);
    }
}

void ScriptMain()
{
    SetConfigs();
    InitializeLogFile();
    
    Ped player = PLAYER::PLAYER_PED_ID();
    lastHealth = ENTITY::GET_ENTITY_HEALTH(player);

	Log("PlayerRagdoll Initialized");

    mainLoop();
}
