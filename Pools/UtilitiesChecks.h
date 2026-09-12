#pragma once

#include "UtilitiesGets.h"
#include "../../inc/natives.h"

inline bool CheckWeaponIsRevolver(Hash weapon)
{
    return WEAPON::IS_WEAPON_REVOLVER(weapon);
}

inline bool CheckWeaponIsPistol(Hash weapon)
{
    return WEAPON::IS_WEAPON_PISTOL(weapon);
}

inline bool CheckWeaponIsRepeater(Hash weapon)
{
    return WEAPON::IS_WEAPON_REPEATER(weapon);
}

inline bool CheckWeaponIsRifle(Hash weapon)
{
    return WEAPON::IS_WEAPON_RIFLE(weapon);
}

inline bool CheckWeaponIsShotgun(Hash weapon)
{
    return WEAPON::IS_WEAPON_SHOTGUN(weapon);
}

inline bool CheckWeaponIsSniper(Hash weapon)
{
    return WEAPON::_IS_WEAPON_SNIPER(weapon);
}

inline bool CheckWeaponIsBow(Hash weapon)
{
    return WEAPON::IS_WEAPON_BOW(weapon);
}

inline bool CheckWeaponIsMelee(Hash weapon)
{
    return WEAPON::IS_WEAPON_MELEE_WEAPON(weapon);
}

inline bool CheckWeaponIsLasso(Hash weapon)
{
    return WEAPON::_IS_WEAPON_LASSO(weapon);
}

inline bool CheckWeaponIsBinoculars(Hash weapon)
{
    return WEAPON::_IS_WEAPON_BINOCULARS(weapon);
}

inline bool CheckWeaponIsLantern(Hash weapon)
{
    return WEAPON::_IS_WEAPON_LANTERN(weapon);
}

inline bool CheckWeaponIsTorch(Hash weapon)
{
    return WEAPON::_IS_WEAPON_TORCH(weapon);
}

inline bool CheckWeaponIsKnife(Hash weapon)
{
    return WEAPON::_IS_WEAPON_KNIFE(weapon);
}

inline bool CheckIsPedRagdolled(Ped ped)
{
    return PED::IS_PED_RAGDOLL(ped);
}

inline bool CheckValidPed(Ped ped)
{
    if (!ped)
        return false;
    if (!ENTITY::DOES_ENTITY_EXIST(ped))
        return false;
    if (ENTITY::IS_ENTITY_DEAD(ped))
        return false;
    return true;
}