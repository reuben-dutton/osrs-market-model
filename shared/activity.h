#ifndef ACTIVITY_INCLUDED
#define ACTIVITY_INCLUDED

#include <stdbool.h>
#include <map>
#include <vector>
#include <string>

typedef std::vector<std::pair<std::string, int>> ItemVector;

class Activity {
    public:
        std::string name;
        int duration; // in ticks, for a single instance
        // e.g. 5 ticks to high alch a single item

        ItemVector requirements;
        ItemVector products;
        int moneyOutput;
        int moneyInput;

        Activity(std::string _name, int _duration);
        void add_requirement(std::string itemName, int itemCount);
        void add_product(std::string itemName, int itemCount);
        void add_money_output(int _moneyOutput);
        void add_money_input(int _moneyInput);
        void print_activity();
        bool has_required_items(std::map<std::string, int> bank, int activeDuration);

        ItemVector get_products(int activeDuration);
        ItemVector get_requirements(int activeDuration);


    private:
        int prevActiveDurationProducts;
        int prevActiveDurationRequirements;
        ItemVector activeRequirements;
        ItemVector activeProducts;

};

class Activities {
    private:
        static Activity* _mineRunite;
        static Activity* _mineCoal;
        static Activity* _smithRuniteBar;
        static Activity* _smithRunePlatebody;
        static Activity* _alchRunePlatebody;
        static Activity* _craftNatureRunes;

    public:
        static Activity* MineRunite();
        static Activity* MineCoal();
        static Activity* SmithRuniteBar();
        static Activity* SmithRunePlatebody();
        static Activity* AlchRunePlatebody();
        static Activity* CraftNatureRunes();
};

#endif