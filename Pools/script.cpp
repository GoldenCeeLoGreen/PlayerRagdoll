#include "script.h"
#include <cmath>
#include <string>
#include <fstream>
#include <sstream>
#include <random>

std::random_device rd;
std::mt19937 rng(rd());

struct HitPacket
{
    bool valid = false;

    Any* event{};

    int bone = -1;

    int startHealth = 0;
    int endHealth = 0;
    int damage = 0;
    float damagePercentFullHealth = 0.0f;
    float damagePercentCurrentHealth = 0.0f;

    Hash equippedWeapon;

    Ped attacker;

    Vector3 direction;

    bool hasBone = false;
    bool hasAttacker = false;
};

const Hash EVENT_ENTITY_DAMAGED = GAMEPLAY::GET_HASH_KEY("EVENT_ENTITY_DAMAGED");

static Ped player;
static int lastHealth = 0;
static HitPacket hitPacket;

static int captureFrames = 0;
static int framesToAttemptForce = 0;
static int ragdollCooldownSetting = 0;
static int forceCooldownSetting = 0;
static int ragdollCooldown = 0;
static int forceCooldown = 0;
static int radollDurationForType0 = 0;
static int radollDurationForType1 = 0;
static int radollDurationForType2 = 0;
static int radollDurationForType3 = 0;
static float healthToCauseType0 = 0;
static float healthToCauseType1 = 0;
static float healthToCauseType2 = 0;
static float healthToCauseType3 = 0;
static float forceBaseToApplyToRagdoll = 0;
static int chanceToRegisterHitSetting = 100;
static bool useTotalHealthForPercentSetting = false;
static bool immunityRevolver = false;
static bool immunityPistol = false;
static bool immunityRepeater = false;
static bool immunityRifle = false;
static bool immunityShotgun = false;
static bool immunitySniper = false;
static bool immunityBow = false;
static bool immunityMelee = false;


template<typename T>
void Log(T value)
{
    static std::ofstream file(".\\PlayerRagdoll.log", std::ios::app);

    file << value << std::endl;
    file.flush();
}

template <typename T>
std::string ToString(const T& value)
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

int Random1To100()
{
    std::uniform_int_distribution<int> dist(1, 100);
    return dist(rng);
}

int GetIniInt(const char* key) 
{
    return GetPrivateProfileIntA("settings", key, 0, ".\\PlayerRagdoll.ini");
}

float GetIniFloat( const char* key)
{
    char buffer[64]{};

    GetPrivateProfileStringA(
        "settings",
        key,
        "0",
        buffer,
        sizeof(buffer),
        ".\\PlayerRagdoll.ini"
    );

    if (buffer[0] == '\0')
        return 0.0f;

    return strtof(buffer, nullptr);
}

Hash GetPlayerWeapon(Ped player)
{
    Hash weaponHash;

    WEAPON::GET_CURRENT_PED_WEAPON(player, &weaponHash, true, 0, false);
    return weaponHash;
}

Hash GetWeaponGroup(Hash weaponHash)
{
    return WEAPON::GET_WEAPONTYPE_GROUP(weaponHash);

}

bool DetectHitAndCapturePLayerStatus(Ped player)
{
    int health = ENTITY::GET_ENTITY_HEALTH(player);

    bool hit = (health < lastHealth);

    if (hit)
    {
        hitPacket.startHealth = lastHealth;
        hitPacket.endHealth = health;

        hitPacket.damage = hitPacket.startHealth - hitPacket.endHealth;

        int maxHealth = ENTITY::GET_ENTITY_MAX_HEALTH(player,true);
        if (maxHealth > 0)
        {
            hitPacket.damagePercentFullHealth =
                (float)hitPacket.damage / (float)maxHealth;
        }
        else
        {
            hitPacket.damagePercentFullHealth = 0.0f;
        }
        if(health > 0)
            hitPacket.damagePercentCurrentHealth = (float)hitPacket.damage / (float)health;

        hitPacket.equippedWeapon = GetPlayerWeapon(player);
    }

    lastHealth = health;

    return hit;
}

void CaptureBoneData(Ped player)
{
    int bone;

    if (PED::GET_PED_LAST_DAMAGE_BONE(player, &bone))
    {
        hitPacket.bone = bone;
        hitPacket.hasBone = true;
    }
}

Vector3 GetPedLocation(Ped ped)
{
    return ENTITY::GET_ENTITY_COORDS(ped, true, false);
}

void ComputeDirection(Ped player, Ped attacker)
{
    Vector3 posPlayer = GetPedLocation(player);
    Vector3 posAttacker = GetPedLocation(attacker);

    Vector3 dir;
    dir.x = posPlayer.x - posAttacker.x;
    dir.y = posPlayer.y - posAttacker.y;
    dir.z = posPlayer.z - posAttacker.z;

    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);

    if (len == 0.0f)
        return;

    dir.x /= len;
    dir.y /= len;
    dir.z /= len;

    hitPacket.direction = dir;
}

int GetReactionFromDamage(float percent)
{
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

bool CheckWeaponIsRevolver(Hash weapon)
{
    return GetWeaponGroup(weapon) == 3193669993;
}

bool CheckWeaponIsPistol(Hash weapon)
{
    return GetWeaponGroup(weapon) == 0x18D5FA97;
}

bool CheckWeaponIsRepeater(Hash weapon)
{
    return GetWeaponGroup(weapon) == 0x39D5C192;
}

bool CheckWeaponIsRifle(Hash weapon)
{
    return GetWeaponGroup(weapon) == 0x0F8A393D;
}

bool CheckWeaponIsShotgun(Hash weapon)
{
    return GetWeaponGroup(weapon) == 0x33431399;
}

bool CheckWeaponIsSniper(Hash weapon)
{
    return GetWeaponGroup(weapon) == 0xB5FD67CD;
}

bool CheckWeaponIsBow(Hash weapon)
{
    return GetWeaponGroup(weapon) == 0xB6DDB23B;
}

bool CheckWeaponIsMelee(Hash weapon)
{
    return GetWeaponGroup(weapon) == 0xD49321D4;
}

bool DetermineShouldRegisterHit()
{
    //Log(GetPlayerWeapon(PLAYER::PLAYER_PED_ID()));
    Log(ToString(GetWeaponGroup(hitPacket.equippedWeapon)));

    Log("Immunity: " + ToString(immunityRevolver));
    Log("Weapon Group: " + ToString(CheckWeaponIsRevolver(hitPacket.equippedWeapon)));

    //if (Random1To100() > chanceToRegisterHitSetting)
    //    return false;

    if (hitPacket.valid = true)
        return false;


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

bool IsPlayerRagdolled(Ped player)
{
    return PED::IS_PED_RAGDOLL(player);
}

void ApplyHitReaction(Ped player)
{
    if (ragdollCooldown > 0) { ragdollCooldown--; }
    if (forceCooldown > 0) { forceCooldown--; }
    if (framesToAttemptForce > 0) { framesToAttemptForce--; }

    if (!hitPacket.valid)
        return;

    bool appliedForce = false;

    //Log("RDCD " + ToString(ragdollCooldown));
    if (!IsPlayerRagdolled(player) && ragdollCooldown == 0)
    {
        int ragdollType = DetermineRagdollType(player);
        //Log("RGT " + ToString(ragdollType));
        if (ragdollType >= 0 && !PED::_0x3BDFCF25B58B0415(player))
        {
            //Log("playerragdolled");
            ApplyRagdoll(player, DetermineDuration(ragdollType), ragdollType);
            framesToAttemptForce = 5;
            ragdollCooldown = ragdollCooldownSetting;
        }
    }

    //Log("FCD " + ToString(forceCooldown));
    if (IsPlayerRagdolled(player) && forceCooldown == 0)
    {

        float intensity = (GetDamagePercentFromHitPacket(useTotalHealthForPercentSetting) * forceBaseToApplyToRagdoll);
        //Log("Force applied");
        ENTITY::APPLY_FORCE_TO_ENTITY(
            player,
            1,
            hitPacket.direction.x * intensity,
            hitPacket.direction.y * intensity,
            hitPacket.direction.z * intensity,
            0.0f, 0.0f, 0.0f,
            hitPacket.bone,
            true,
            true,
            true,
            false,
            false
        );
        appliedForce = true;
        forceCooldown = forceCooldownSetting;
    }

    if ( appliedForce || framesToAttemptForce == 0)
    {
        //Log("clearedhitpacket");
        ClearHitPacket();
    }
}

Ped GetClosestCombatPed(Ped player)
{
    int set = ITEMSET::CREATE_ITEMSET(true);

    PED::_0x7BE607DAFF382FD2(player, set, 0);

    int size = ITEMSET::GET_ITEMSET_SIZE(set);

    Ped closest = 0;
    float bestDist = 999999.0f;

    Vector3 playerPos = ENTITY::GET_ENTITY_COORDS(player, true, false);

    for (int i = 0; i < size; i++)
    {
        Entity ent = ITEMSET::GET_INDEXED_ITEM_IN_ITEMSET(i, set);
        Ped ped = (Ped)ent;

        if (!ENTITY::DOES_ENTITY_EXIST(ped))
            continue;

        if (ped == player)
            continue;

        if (!WEAPON::_0xCB690F680A3EA971(ped, 2))
            continue;

        if (!ENTITY::HAS_ENTITY_BEEN_DAMAGED_BY_ENTITY(player, ped, false, false))
            continue;

        Vector3 pedPos = ENTITY::GET_ENTITY_COORDS(ped, true, false);

        float dist = SYSTEM::VDIST(
            playerPos.x, playerPos.y, playerPos.z,
            pedPos.x, pedPos.y, pedPos.z
        );

        if (dist < bestDist)
        {
            bestDist = dist;
            closest = ped;
        }
    }

    ITEMSET::DESTROY_ITEMSET(set);

    return closest;
}

void CaptureAttackerData(Ped player)
{
    hitPacket.attacker = GetClosestCombatPed(player);
    hitPacket.hasAttacker = true;
}

void UpdateHitSystem(Ped player)
{
    if (captureFrames == 0)
    {
        ClearHitPacket();
        bool hit = DetectHitAndCapturePLayerStatus(player);
        if (hit)
        {
            Log("Determine: " + ToString(DetermineShouldRegisterHit()));
            if (DetermineShouldRegisterHit())
            {
                //Log("Player hit");
                captureFrames = 10;
            }
        }
    }

    if (captureFrames > 0)
    {
        if (!hitPacket.hasAttacker) { CaptureAttackerData(player); }
        if (!hitPacket.hasBone) { CaptureBoneData(player); }
        if (hitPacket.hasAttacker) { ComputeDirection(player, hitPacket.attacker); }
        captureFrames--;

        if (hitPacket.hasBone && hitPacket.hasAttacker)
        {
            //Log("Hitpacketvalid");
            hitPacket.valid = true;
            captureFrames = 0;
        }
    }
}

bool CheckValidPlayer()
{
    if (!player)
        return false;
    if (!ENTITY::DOES_ENTITY_EXIST(player))
        return false;
    if (!ENTITY::IS_ENTITY_DEAD(player))
        return false;
    return true;
}

void GetDamageEventData()
{
    int eventCount = SCRIPT::GET_NUMBER_OF_EVENTS(0);

    for (int i = 0; i < eventCount; ++i)
    {
        Hash eventType = SCRIPT::GET_EVENT_AT_INDEX(0, i);
        if (eventType != EVENT_ENTITY_DAMAGED)
            continue;

        if (!SCRIPT::GET_EVENT_DATA(0, i, &event, 9))
            continue;
    }
}

bool ConvertIntToBool(int input)
{
    return input == 1;
}

void main()
{
    while (true)
    {
        player = PLAYER::PLAYER_PED_ID();

        GetDamageEventData();
        UpdateHitSystem(player);
        ApplyHitReaction(player);

        WAIT(0);
    }
}

void ScriptMain()
{
    std::ofstream file(".\\PlayerRagdoll.log", std::ios::trunc);
    file.close();

    Ped player = PLAYER::PLAYER_PED_ID();
    lastHealth = ENTITY::GET_ENTITY_HEALTH(player);

    (int)ragdollCooldownSetting = GetIniFloat("ragdollCooldown") * 1000;
    (int)forceCooldownSetting = GetIniFloat("forceCooldown") * 1000;

    (int)radollDurationForType0 = GetIniFloat("radollDurationForType0") * 1000;
    (int)radollDurationForType1 = GetIniFloat("radollDurationForType1") * 1000;
    (int)radollDurationForType2 = GetIniFloat("radollDurationForType2") * 1000;
    (int)radollDurationForType3 = GetIniFloat("radollDurationForType3") * 1000;

    healthToCauseType0 = GetIniFloat("healthToCauseType0") / 100;
    healthToCauseType1 = GetIniFloat("healthToCauseType1") / 100;
    healthToCauseType2 = GetIniFloat("healthToCauseType2") / 100;
    healthToCauseType3 = GetIniFloat("healthToCauseType3") / 100;

    forceBaseToApplyToRagdoll = GetIniFloat("forceBaseToApplyToRagdoll");

    chanceToRegisterHitSetting = GetIniFloat("chanceToRegisterHit");

    useTotalHealthForPercentSetting = ConvertIntToBool(GetIniInt("useTotalHealthForPercent"));

    immunityRevolver = ConvertIntToBool(GetIniInt("immunityRevolver"));
    immunityPistol = ConvertIntToBool(GetIniInt("immunityPistol"));
    immunityRepeater = ConvertIntToBool(GetIniInt("immunityRepeater"));
    immunityRifle = ConvertIntToBool(GetIniInt("immunityRifle"));
    immunityShotgun = ConvertIntToBool(GetIniInt("immunityShotgun"));
    immunitySniper = ConvertIntToBool(GetIniInt("immunitySniper"));
    immunityBow = ConvertIntToBool(GetIniInt("immunityBow"));
    immunityMelee = ConvertIntToBool(GetIniInt("immunityMelee"));

    const Hash EVENT_ENTITY_DAMAGED = GET_HASH_KEY("EVENT_ENTITY_DAMAGED");

    main();
}