#ifndef SKILLS_INCLUDED
#define SKILLS_INCLUDED

#include <stdbool.h>
#include <string>

class Skill {
    public:
        bool members;
        std::string name;

        Skill(bool isMembers, std::string skillName);
};

// Free-to-play skills
extern Skill Attack;
extern Skill Strength;
extern Skill Defence;
extern Skill Ranged;
extern Skill Prayer;
extern Skill Magic;
extern Skill Runecraft;
extern Skill Hitpoints;
extern Skill Crafting;
extern Skill Mining;
extern Skill Smithing;
extern Skill Fishing;
extern Skill Cooking;
extern Skill Firemaking;
extern Skill Woodcutting;
// Members skills
extern Skill Agility;
extern Skill Herblore;
extern Skill Thieving;
extern Skill Fletching;
extern Skill Slayer;
extern Skill Farming;
extern Skill Construction;
extern Skill Hunter;

#endif