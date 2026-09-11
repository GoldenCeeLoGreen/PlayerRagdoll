#pragma once

#include "UtilitiesGets.h"
#include "../../inc/natives.h"

inline bool CheckWeaponIsRevolver(Hash weapon)
{
    return WEAPON::_0xC212F1D05A8232BB(weapon);
}

inline bool CheckWeaponIsPistol(Hash weapon)
{
    return WEAPON::_0xDDC64F5E31EEDAB6(weapon);
}

inline bool CheckWeaponIsRepeater(Hash weapon)
{
    return WEAPON::_0xDDB2578E95EF7138(weapon);
}

inline bool CheckWeaponIsRifle(Hash weapon)
{
    return WEAPON::_0x0A82317B7EBFC420(weapon);
}

inline bool CheckWeaponIsShotgun(Hash weapon)
{
    return WEAPON::_0xC75386174ECE95D5(weapon);
}

inline bool CheckWeaponIsSniper(Hash weapon)
{
    return WEAPON::_0x6AD66548840472E5(weapon);
}

inline bool CheckWeaponIsBow(Hash weapon)
{
    return WEAPON::_0xC4DEC3CA8C365A5D(weapon);
}

inline bool CheckWeaponIsMelee(Hash weapon)
{
    return WEAPON::_0x959383DCD42040DA(weapon);
}

inline bool CheckWeaponIsLasso(Hash weapon)
{
    return WEAPON::_0x6E4E1A82081EABED(weapon);
}

inline bool CheckWeaponIsBinoculars(Hash weapon)
{
    return WEAPON::_0xC853230E76A152DF(weapon);
}

inline bool CheckWeaponIsLantern(Hash weapon)
{
    return WEAPON::_0x79407D33328286C6(weapon);
}

inline bool CheckWeaponIsTorch(Hash weapon)
{
    return WEAPON::_0x506F1DE1BFC75304(weapon);
}

inline bool CheckWeaponIsKnife(Hash weapon)
{
    return WEAPON::_0x792E3EF76C911959(weapon);
}

inline bool CheckIsPedRagdolled(Ped ped)
{
    return PED::IS_PED_RAGDOLL(ped);
}

inline bool CheckValidPlayer(Ped player)
{
    if (!player)
        return false;
    if (!ENTITY::DOES_ENTITY_EXIST(player))
        return false;
    if (!ENTITY::IS_ENTITY_DEAD(player))
        return false;
    return true;
}