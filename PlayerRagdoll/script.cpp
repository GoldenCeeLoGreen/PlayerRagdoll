#include "Globals.h"

#include <cmath>

using namespace GETSPEDINFO;

// How many frames after the ragdoll we keep trying to apply the force effect
constexpr int FRAMES_TO_ATTEMPT_FORCE_EFFECT = 30;

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

    // dir points attacker -> player, so the force pushes the player away from the attacker
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
    PED::SET_PED_TO_RAGDOLL(player, duration, duration, ragdolltype, true, true, nullptr);
}

void ApplyForce(Ped ped, Vector3 direction, float intensity)
{
    Log("Force applied to center of mass with intensity " + ToString(intensity));
    ENTITY::APPLY_FORCE_TO_ENTITY_CENTER_OF_MASS(
        ped,
        1,
        direction.x * intensity,
        direction.y * intensity,
        direction.z * intensity,
        0,
        false, // world-space direction, not relative to the ped's orientation
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
    return GetReactionFromDamage(GetDamagePercentFromHitPacket());
}

static bool Immune(const char* reason)
{
    Log(std::string(reason) + " is immune to effects");
    return true;
}

bool CheckImmunity()
{
    Log("Checking for immune weapon");

    const Hash weapon = hitPacket.equippedWeapon;

    if (settingImmunityRevolver && CheckWeaponIsRevolver(weapon)) return Immune("Revolver");
    if (settingImmunityPistol && CheckWeaponIsPistol(weapon)) return Immune("Pistol");
    if (settingImmunityRepeater && CheckWeaponIsRepeater(weapon)) return Immune("Repeater");
    if (settingImmunityRifle && CheckWeaponIsRifle(weapon)) return Immune("Rifle");
    if (settingImmunityShotgun && CheckWeaponIsShotgun(weapon)) return Immune("Shotgun");
    if (settingImmunitySniper && CheckWeaponIsSniper(weapon)) return Immune("Sniper");
    if (settingImmunityBow && CheckWeaponIsBow(weapon)) return Immune("Bow");
    if (settingImmunityMelee && CheckWeaponIsMelee(weapon)) return Immune("Melee");
    if (settingImmunityLasso && CheckWeaponIsLasso(weapon)) return Immune("Lasso");
    if (settingImmunityBinoculars && CheckWeaponIsBinoculars(weapon)) return Immune("Binoculars");
    if (settingImmunityLantern && CheckWeaponIsLantern(weapon)) return Immune("Lantern");
    if (settingImmunityTorch && CheckWeaponIsTorch(weapon)) return Immune("Torch");
    if (settingImmunityKnife && CheckWeaponIsKnife(weapon)) return Immune("Knife");

    if (settingImmunityHorse && CheckPedIsOnHorse(player)) return Immune("Horse");
    if (settingImmunityVehicle && CheckPedIsOnVehicle(player)) return Immune("Vehicle");
    if (settingImmunityGrapple && PED::GET_PED_IS_GRAPPLING(player)) return Immune("Grapple");
    if (settingImmunityMeleeCombat && PED::IS_PED_IN_MELEE_COMBAT(player)) return Immune("Melee Combat");

    Log("No immunities found");
    return false;
}

bool DetermineShouldApplyRagdoll()
{
    if (hitPacket.ragdollApplied)
    {
        Log("Ragdoll has already been applied for this hit event");
        return false;
    }

    if (timeLastRagdoll != 0 && !GETSTIME::HasTimePassed(timeLastRagdoll, settingCooldownRagdoll))
    {
        Log("Ragdoll is on cooldown for " + ToString(settingCooldownRagdoll - GETSTIME::GetTimeSince(timeLastRagdoll)) + " more ms");
        return false;
    }

    Log("Ragdoll should be applied");
    return true;
}

bool DetermineShouldForceEffect()
{
    if (!hitPacket.ragdollApplied)
    {
        Log("Ragdoll has not been applied for this hit event");
        return false;
    }

    if (timeLastForceEffect != 0 && !GETSTIME::HasTimePassed(timeLastForceEffect, settingCooldownForceEffect))
    {
        Log("Force Effect is on cooldown for " + ToString(settingCooldownForceEffect - GETSTIME::GetTimeSince(timeLastForceEffect)) + " more ms");
        return false;
    }

    if (hitPacket.ragdollType < 0 || hitPacket.ragdollType > 3)
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
            return settingRagdollDurationForType0;
        case 1:
            return settingRagdollDurationForType1;
        case 2:
            return settingRagdollDurationForType2;
        case 3:
            return settingRagdollDurationForType3;
        default:
            return 0;
    }
}

float DetermineForceIntensity()
{
    float intensityBase = settingForceBaseToApplyToRagdoll;
    Log("Intensity base: " + ToString(intensityBase));
    // Damage percent is a 0-1 fraction, so this scales the force from 90% to 100% of base
    float scaledPercent = (GetDamagePercentFromHitPacket() * 10.0f) + 90.0f;
    Log("Scaled percent: " + ToString(scaledPercent) + "Based on damage percent" + ToString(GetDamagePercentFromHitPacket()));
    float finalIntensity = intensityBase * (scaledPercent / 100.0f);
    Log("Final intensity: " + ToString(finalIntensity));
    return finalIntensity;
}

bool DetermineShouldClearHitPacket()
{
    if (hitPacket.forceApplied)
    {
        Log("Force has been applied, clearing hit packet");
        return true;
    }

    if (settingRequireHumanDamageSource)
    {
        if (!PED::IS_PED_HUMAN(hitPacket.attacker))
        {
            Log("Damage source is not human, clearing hit packet");
            return true;
        }
    }

    if (settingRequireGunDamageSource)
    {
        if (!WEAPON::IS_WEAPON_A_GUN(hitPacket.attackerWeapon))
        {
            Log("Damage source weapon not a gun");
            return true;
        }
    }

    if (CheckImmunity())
    {
        Log("Immunity Found");
        return true;
    }

    if (hitPacket.ragdollType < 0)
    {
        Log("Damage is below every ragdoll threshold, clearing hit packet");
        return true;
    }

    if (!hitPacket.rollPassed)
    {
        Log("Roll did not pass, clearing hit packet");
        return true;
    }

    if (framesToAttemptForceEffect == 0 && hitPacket.ragdollApplied)
    {
        Log("Frames to attempt force effect has reached 0, clearing hit packet");
        return true;
    }

    // Only before our own ragdoll is applied, otherwise it would clear the packet before the force effect
    if (settingIgnoreHitsWhileRagdolled && !hitPacket.ragdollApplied && CheckIsPedRagdolled(player))
    {
        Log("Player is already ragdolled, clearing hit packet");
        return true;
    }

    Log("Hit packet should not be cleared yet");
    return false;
}

void ApplyHitReaction()
{
    if (DetermineShouldClearHitPacket())
    {
        ClearHitPacket();
        return;
    }

    if (DetermineShouldApplyRagdoll())
    {
        ApplyRagdoll(player, hitPacket.ragdollDuration, hitPacket.ragdollType);
        framesToAttemptForceEffect = FRAMES_TO_ATTEMPT_FORCE_EFFECT;
        timeLastRagdoll = GETSTIME::GetGameTime();
        hitPacket.ragdollApplied = true;
    }

    Log("framesToAttemptForceEffect " + ToString(framesToAttemptForceEffect));
    if (DetermineShouldForceEffect())
    {
        ApplyForce(player, hitPacket.attackerDirection, hitPacket.forceIntensity);
        hitPacket.forceApplied = true;
        timeLastForceEffect = GETSTIME::GetGameTime();
    }
}

void UpdateHitSystem()
{
    Log("Updating Hit System");

    hitPacket.equippedWeapon = GetPedWeapon(player);
    Log("Event equippedWeapon Logged: " + ToString(hitPacket.equippedWeapon));
    Log("Event equippedWeapon Name Logged: " + ToString(WEAPON::_GET_WEAPON_NAME(hitPacket.equippedWeapon)));

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
    hitPacket.attackerDirection = ComputeDirection(player, hitPacket.attacker);
    Log("Event direction Logged: " + ToString(hitPacket.attackerDirection.x) + ", " + ToString(hitPacket.attackerDirection.y) + ", " + ToString(hitPacket.attackerDirection.z));
    hitPacket.attackerWeapon = GetPedWeapon(hitPacket.attacker);
    Log("Event attackerWeapon Logged: " + ToString(hitPacket.attackerWeapon));
    Log("Event attackerWeapon Name Logged: " + ToString(WEAPON::_GET_WEAPON_NAME(hitPacket.attackerWeapon)));

    hitPacket.forceIntensity = DetermineForceIntensity();
    Log("Event forceIntensity Logged: " + ToString(hitPacket.forceIntensity));

    int roll = Random1To100();
    hitPacket.rollPassed = roll <= settingChanceToApplyEffects;
    Log("Event roll Logged: " + ToString(roll) + " Roll Passed: " + ToString(hitPacket.rollPassed));

    if (settingMeanShotguns)
    {
        if (CheckWeaponIsShotgun(hitPacket.attackerWeapon))
        {
            Log("Mean Shotgun: increasing effects");
            if (hitPacket.ragdollType < 0)
                hitPacket.ragdollType = 3;
            else if (hitPacket.ragdollType > 0)
                hitPacket.ragdollType--;
            hitPacket.ragdollDuration = DetermineDuration(hitPacket.ragdollType);
        }
    }

    hitPacket.valid = true;
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

        if (!SCRIPT::GET_EVENT_DATA(0, i, &event, 9))
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
    if (framesToAttemptForceEffect > 0) { framesToAttemptForceEffect--; }
}

void mainLoop()
{
    while (true)
    {
        player = PLAYER::PLAYER_PED_ID();
        DecrementCooldowns();
        if (CheckValidPed(player))
        {
            if (!hitPacket.validEvent) { GetDamageEventData(); }
            if (hitPacket.validEvent && !hitPacket.valid) { UpdateHitSystem(); }
            if (hitPacket.valid) { ApplyHitReaction(); }
        }
        lastHealth = ENTITY::GET_ENTITY_HEALTH(player);
        WAIT(0);
    }
}

void ScriptMain()
{
    fileNameSettings = ".\\PlayerRagdoll.ini";
    LogFileName = ".\\PlayerRagdoll.log";

    SetConfigs();
    InitializeLogFile();

    player = PLAYER::PLAYER_PED_ID();
    lastHealth = ENTITY::GET_ENTITY_HEALTH(player);

    Log("PlayerRagdoll Initialized");

    mainLoop();
}

BOOL APIENTRY DllMain(HMODULE hInstance, DWORD reason, LPVOID lpReserved)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        scriptRegister(hInstance, ScriptMain);
    }

    if (reason == DLL_PROCESS_DETACH)
    {
        scriptUnregister(hInstance);
    }

    return TRUE;
}
