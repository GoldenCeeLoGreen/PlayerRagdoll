#include "Globals.h"

#include <cmath>

using namespace GETSPEDINFO;

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

RagdollType GetReactionFromDamage(float percent)
{
    Log("Determining reaction type for damage percent: " + ToString(percent));
    if (percent > settingHealthPercentToCauseType0)
        return RagdollType::Normal;
    if (percent > settingHealthPercentToCauseType1)
        return RagdollType::StiffFall;
    if (percent > settingHealthPercentToCauseType3)
        return RagdollType::WideStumble;
    return RagdollType::None;
}

RagdollType GetStrongerReaction(RagdollType reaction)
{
    switch (reaction)
    {
        case RagdollType::WideStumble:
            return RagdollType::StiffFall;
        case RagdollType::StiffFall:
        case RagdollType::Normal:
            return RagdollType::Normal;
        default:
            return RagdollType::WideStumble;
    }
}

// Value taken from how the game scripts call SET_PED_TO_RAGDOLL_WITH_FALL; the remaining extra arguments are 0
constexpr float FALL_EXTRA_P7 = 2.0f;

void ApplyStandardRagdoll(Ped player, int duration, RagdollType ragdollType)
{
    BOOL result = PED::SET_PED_TO_RAGDOLL(player, duration, duration, (int)ragdollType, false, false, "");
    Log("SET_PED_TO_RAGDOLL returned: " + ToString(result) + " IS_PED_RAGDOLL right after: " + ToString(PED::IS_PED_RAGDOLL(player)));
}

void ApplyFallRagdoll(Ped player, int duration, RagdollType ragdollType, Vector3 fallDirection)
{
    BOOL result = PED::SET_PED_TO_RAGDOLL_WITH_FALL(player, duration, duration, (int)ragdollType,
        fallDirection.x, fallDirection.y, 0.0f, FALL_EXTRA_P7, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    Log("SET_PED_TO_RAGDOLL_WITH_FALL returned: " + ToString(result) + " IS_PED_RAGDOLL right after: " + ToString(PED::IS_PED_RAGDOLL(player)));
}

// Ragdoll with fall is ignored while sprinting, so only the strong types use it and only when not sprinting
bool ShouldUseFallRagdoll(Ped player, RagdollType ragdollType)
{
    if (ragdollType != RagdollType::Normal && ragdollType != RagdollType::StiffFall)
        return false;

    if (CheckIsPedSprinting(player))
    {
        Log("Player is sprinting, using the standard ragdoll instead of ragdoll with fall");
        return false;
    }

    return true;
}

void ApplyRagdoll(Ped player, int duration, RagdollType ragdollType, Vector3 fallDirection)
{
    Log("Player Ragdolled for " + ToString(duration) + "ms with type " + ToString((int)ragdollType));
    Log("CAN_PED_RAGDOLL before: " + ToString(PED::CAN_PED_RAGDOLL(player)));

    if (ShouldUseFallRagdoll(player, ragdollType))
        ApplyFallRagdoll(player, duration, ragdollType, fallDirection);
    else
        ApplyStandardRagdoll(player, duration, ragdollType);
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

RagdollType DetermineRagdollType()
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

    if (settingImmunityDeadEye && IsDeadEyeActivated(PLAYER::PLAYER_ID())) return Immune("Dead Eye");
    if (settingImmunityCarrying && PED::IS_PED_CARRYING_SOMETHING(player)) return Immune("Carrying");

    Log("No immunities found");
    return false;
}

bool DetermineShouldApplyRagdoll()
{
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
    if (!CheckIsPedRagdolled(player))
    {
        Log("Player is not ragdolled");
        return false;
    }

    if (timeLastForceEffect != 0 && !GETSTIME::HasTimePassed(timeLastForceEffect, settingCooldownForceEffect))
    {
        Log("Force Effect is on cooldown for " + ToString(settingCooldownForceEffect - GETSTIME::GetTimeSince(timeLastForceEffect)) + " more ms");
        return false;
    }

    Log("Force effect should be applied");
    return true;
}

int DetermineDuration(RagdollType ragdollType)
{
    switch (ragdollType)
    {
        case RagdollType::Normal:
            return settingRagdollDurationForType0;
        case RagdollType::StiffFall:
            return settingRagdollDurationForType1;
        case RagdollType::WideStumble:
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

    if (hitPacket.ragdollType == RagdollType::None)
    {
        Log("Damage is below every ragdoll threshold, clearing hit packet");
        return true;
    }

    if (!hitPacket.rollPassed)
    {
        Log("Roll did not pass, clearing hit packet");
        return true;
    }

    if (settingIgnoreHitsWhileRagdolled)
    {
        if (CheckIsPedRagdolled(player))
        {
            Log("Player is already ragdolled, clearing hit packet");
            return true;
        }
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
        ApplyRagdoll(player, hitPacket.ragdollDuration, hitPacket.ragdollType, hitPacket.attackerDirection);
        timeLastRagdoll = GETSTIME::GetGameTime();
    }

    if (DetermineShouldForceEffect())
    {
        ApplyForce(player, hitPacket.attackerDirection, hitPacket.forceIntensity);
        timeLastForceEffect = GETSTIME::GetGameTime();
    }

    ClearHitPacket();
}

void UpdateHitSystem()
{
    Log("Updating Hit System");

    hitPacket.equippedWeapon = GetPedWeapon(player);
    Log("Event equippedWeapon Logged: " + ToString(hitPacket.equippedWeapon));
    Log("Event equippedWeapon Name Logged: " + ToString(WEAPON::_GET_WEAPON_NAME(hitPacket.equippedWeapon)));

    hitPacket.fullHealth = ENTITY::GET_ENTITY_MAX_HEALTH(player, 0) - (100 - ATTRIBUTE::_GET_ATTRIBUTE_CORE_VALUE(player, 0));
    Log("Event fullHealth Logged: " + ToString(hitPacket.fullHealth));
    hitPacket.damage = hitPacket.event.damage;
    Log("Event Damage Logged: " + ToString(hitPacket.damage));
    hitPacket.startHealth = lastHealth;
    Log("Event startHealth Logged: " + ToString(hitPacket.startHealth));
    Log("Event currentHealth Logged: " + ToString(ENTITY::GET_ENTITY_HEALTH(player)));
    hitPacket.damagePercentFullHealth = hitPacket.damage / MinOne(hitPacket.fullHealth);
    Log("Event damagePercentFullHealth Logged: " + ToString(hitPacket.damagePercentFullHealth));
    hitPacket.damagePercentCurrentHealth = hitPacket.damage / MinOne(hitPacket.startHealth);
    Log("Event damagePercentCurrentHealth Logged: " + ToString(hitPacket.damagePercentCurrentHealth));

    hitPacket.ragdollType = DetermineRagdollType();
    Log("Event ragdollType Logged: " + ToString((int)hitPacket.ragdollType));
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
            hitPacket.ragdollType = GetStrongerReaction(hitPacket.ragdollType);
            hitPacket.ragdollDuration = DetermineDuration(hitPacket.ragdollType);
            hitPacket.forceIntensity *= 2.0f;
            Log("Mean Shotgun: doubled forceIntensity to " + ToString(hitPacket.forceIntensity));
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

// Debug: numpad 1 cycles SET_PED_TO_RAGDOLL_WITH_FALL through types 3, 2, 1, 0 using one set of random criteria,
// then rolls new criteria on the next press.
constexpr bool DEBUG_FALL_HOTKEY = true;

struct DebugFallCriteria
{
    int duration = 0;
    float dirX = 0.0f, dirY = 0.0f, dirZ = 0.0f;
    float p7 = 0.0f, p8 = 0.0f, p9 = 0.0f, p10 = 0.0f, p11 = 0.0f, p12 = 0.0f, p13 = 0.0f;
};

DebugFallCriteria GenerateDebugFallCriteria()
{
    DebugFallCriteria c;

    c.duration = (int)RandomRange(500.0f, 3000.0f);

    // Random horizontal unit direction with a small vertical component
    float angle = RandomRange(0.0f, 6.2831853f);
    c.dirX = std::cos(angle);
    c.dirY = std::sin(angle);
    c.dirZ = RandomRange(-0.3f, 0.3f);
    float len = std::sqrt(c.dirX * c.dirX + c.dirY * c.dirY + c.dirZ * c.dirZ);
    c.dirX /= len;
    c.dirY /= len;
    c.dirZ /= len;

    c.p7 = RandomRange(0.0f, 5.0f);
    c.p8 = RandomRange(-1.0f, 1.0f);
    c.p9 = RandomRange(-1.0f, 1.0f);
    c.p10 = RandomRange(-1.0f, 1.0f);
    c.p11 = RandomRange(-1.0f, 1.0f);
    c.p12 = RandomRange(-1.0f, 1.0f);
    c.p13 = RandomRange(-1.0f, 1.0f);

    return c;
}

void DebugFallRagdoll()
{
    static const int types[] = { 3, 2, 1, 0 };
    static int step = 0;
    static DebugFallCriteria criteria;

    if (!IsKeyPressed(VK_NUMPAD1))
        return;

    if (step == 0)
    {
        criteria = GenerateDebugFallCriteria();
        Log("_____________________");
        Log("DEBUG FALL new criteria: duration=" + ToString(criteria.duration) +
            " dir=(" + ToString(criteria.dirX) + ", " + ToString(criteria.dirY) + ", " + ToString(criteria.dirZ) + ")" +
            " p7=" + ToString(criteria.p7) + " p8=" + ToString(criteria.p8) + " p9=" + ToString(criteria.p9) +
            " p10=" + ToString(criteria.p10) + " p11=" + ToString(criteria.p11) + " p12=" + ToString(criteria.p12) +
            " p13=" + ToString(criteria.p13));
    }

    int type = types[step];
    BOOL result = PED::SET_PED_TO_RAGDOLL_WITH_FALL(player, criteria.duration, criteria.duration, type,
        criteria.dirX, criteria.dirY, criteria.dirZ,
        criteria.p7, criteria.p8, criteria.p9, criteria.p10, criteria.p11, criteria.p12, criteria.p13);
    Log("DEBUG FALL type " + ToString(type) + " returned: " + ToString(result) +
        " IS_PED_RAGDOLL right after: " + ToString(PED::IS_PED_RAGDOLL(player)));

    step = (step + 1) % 4;
}

void mainLoop()
{
    while (true)
    {
        player = PLAYER::PLAYER_PED_ID();
        if (CheckValidPed(player))
        {
            if (DEBUG_FALL_HOTKEY) { DebugFallRagdoll(); }
            if (!hitPacket.validEvent) { GetDamageEventData(); }
            if (hitPacket.validEvent && !hitPacket.valid) { UpdateHitSystem(); }
            if (hitPacket.valid) { ApplyHitReaction(); }
        }
        else if (hitPacket.validEvent)
        {
            Log("Player is not valid, clearing hit packet");
            ClearHitPacket();
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
