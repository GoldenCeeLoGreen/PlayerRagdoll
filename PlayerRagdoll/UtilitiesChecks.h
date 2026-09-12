#pragma once

#include "UtilitiesGets.h"
#include "../../inc/natives.h"

bool CheckWeaponIsRevolver(Hash weapon)
{
    return WEAPON::IS_WEAPON_REVOLVER(weapon);
}

bool CheckWeaponIsPistol(Hash weapon)
{
    return WEAPON::IS_WEAPON_PISTOL(weapon);
}

bool CheckWeaponIsRepeater(Hash weapon)
{
    return WEAPON::IS_WEAPON_REPEATER(weapon);
}

bool CheckWeaponIsRifle(Hash weapon)
{
    return WEAPON::IS_WEAPON_RIFLE(weapon);
}

bool CheckWeaponIsShotgun(Hash weapon)
{
    return WEAPON::IS_WEAPON_SHOTGUN(weapon);
}

bool CheckWeaponIsSniper(Hash weapon)
{
    return WEAPON::_IS_WEAPON_SNIPER(weapon);
}

bool CheckWeaponIsBow(Hash weapon)
{
    return WEAPON::IS_WEAPON_BOW(weapon);
}

bool CheckWeaponIsMelee(Hash weapon)
{
    return WEAPON::IS_WEAPON_MELEE_WEAPON(weapon);
}

bool CheckWeaponIsLasso(Hash weapon)
{
    return WEAPON::_IS_WEAPON_LASSO(weapon);
}

bool CheckWeaponIsBinoculars(Hash weapon)
{
    return WEAPON::_IS_WEAPON_BINOCULARS(weapon);
}

bool CheckWeaponIsLantern(Hash weapon)
{
    return WEAPON::_IS_WEAPON_LANTERN(weapon);
}

bool CheckWeaponIsTorch(Hash weapon)
{
    return WEAPON::_IS_WEAPON_TORCH(weapon);
}

bool CheckWeaponIsKnife(Hash weapon)
{
    return WEAPON::_IS_WEAPON_KNIFE(weapon);
}

bool CheckIsPedRagdolled(Ped ped)
{
    return PED::IS_PED_RAGDOLL(ped);
}

bool CheckValidPed(Ped ped)
{
    if (!ped)
        return false;
    if (!ENTITY::DOES_ENTITY_EXIST(ped))
        return false;
    if (ENTITY::IS_ENTITY_DEAD(ped))
        return false;
    return true;
}