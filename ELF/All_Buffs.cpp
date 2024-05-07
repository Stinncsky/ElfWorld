#include "buff.h"
Buff buffs[] = {
    Buff("poison", -4, 0, 0, 0, 0, 0, MAX_BATTLE_TIME, 1),
    Buff("heal", 20, 0, 0, 0, 0, 0, 10, 1),
    Buff("burn", -6, 0, 0, 0, 0, 0, 20, 1),
    Buff("freeze", -2, 0, -1, 0, 0.1, 0, 30, 1),
    Buff("lightning", -3, 0, 0, 0.1, 0, 0, 30, 1),
    Buff("wind", 0, 0, 0, 0.06, -0.2, 0.05, MAX_BATTLE_TIME, 1),
    Buff("stone", 0, 0, 10, 0, 0, 0, MAX_BATTLE_TIME, 1),
    Buff("hormone", 0, 30, 0, 0, 0, 0.05, 20, 1),
    Buff("weak", 0, -5, 0, 0, 0, 0, MAX_BATTLE_TIME, 1),
    Buff("bleed", -20, 0, -5, 0, 0, 0, 10, 1),
    Buff("buildup", 0, 5, 0, 0, 0, 0, MAX_BATTLE_TIME, 1),
    Buff("spring", 5, 0, 0, 0, 0, 0, MAX_BATTLE_TIME, 0),
    Buff("slowdown", 0, 0, 0, -0.1, -0.1, 0, MAX_BATTLE_TIME, 1),
    Buff("fly", 0, 0, 0, 0.1, 0, 0.1, MAX_BATTLE_TIME, 1),
    Buff("stun", 0, 0, 0, 0, 100, 0, 10, 1)
};
