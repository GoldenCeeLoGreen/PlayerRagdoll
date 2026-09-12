#pragma once

#include "../../inc/natives.h"

Hash GetPedWeapon(Ped ped)
{
    Hash weaponHash = 0;

    WEAPON::GET_CURRENT_PED_WEAPON(
        ped,
        &weaponHash,
        true,
        0,
        false
    );

    return weaponHash;
}

Hash GetWeaponGroup(Hash weaponHash)
{
    return WEAPON::GET_WEAPONTYPE_GROUP(weaponHash);
}

Vector3 GetPedLocation(Ped ped)
{
    return ENTITY::GET_ENTITY_COORDS(
        ped,
        true,
        false
    );
}