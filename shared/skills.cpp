#define INIT_F2P_SKILL(name) Skill name = Skill(false, "name")
#define INIT_P2P_SKILL(name) Skill name = Skill(true, "name")

#include <stdio.h>
#include <cmath>

#include "skills.h"

int expDiff(int level) {
    return 0.25f*floor(level - 1 + 300*(powf(2, (level-1)/7)));
}

Skill::Skill(bool isMembers, const char* skillName) {
    members = isMembers;
    name = skillName;
}

// Initialize free-to-play skills
INIT_F2P_SKILL(Attack);
INIT_F2P_SKILL(Strength);
INIT_F2P_SKILL(Defence);
INIT_F2P_SKILL(Ranged);
INIT_F2P_SKILL(Prayer);
INIT_F2P_SKILL(Magic);
INIT_F2P_SKILL(Runecraft);
INIT_F2P_SKILL(Hitpoints);
INIT_F2P_SKILL(Crafting);
INIT_F2P_SKILL(Mining);
INIT_F2P_SKILL(Smithing);
INIT_F2P_SKILL(Fishing);
INIT_F2P_SKILL(Cooking);
INIT_F2P_SKILL(Firemaking);
INIT_F2P_SKILL(Woodcutting);
// Initialize members skills
INIT_P2P_SKILL(Agility);
INIT_P2P_SKILL(Herblore);
INIT_P2P_SKILL(Thieving);
INIT_P2P_SKILL(Fletching);
INIT_P2P_SKILL(Slayer);
INIT_P2P_SKILL(Farming);
INIT_P2P_SKILL(Construction);
INIT_P2P_SKILL(Hunter);