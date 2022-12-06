#ifndef ACTIVITY_INCLUDED
#define ACTIVITY_INCLUDED

#include <stdbool.h>
#include <map>
#include <vector>

class Activity {
    public:
        const char* name;
        std::vector<const char*> requirements;
        std::vector<const char*> products;
        int moneyOutput;
        int moneyInput;

        Activity(const char* _name);
        void add_requirement(const char* itemName);
        void add_product(const char* itemName);
        void add_money_output(int _moneyOutput);
        void add_money_input(int _moneyInput);
        void print_activity();
        bool has_required_items(std::map<const char*, int> bank);
};

class Activities {
    private:
        static Activity* _mineRunite;
        static Activity* _mineCoal;
        static Activity* _smithRuniteBar;
        static Activity* _smithRunePlatebody;
        static Activity* _alchRunePlatebody;

    public:
        static Activity* MineRunite();
        static Activity* MineCoal();
        static Activity* SmithRuniteBar();
        static Activity* SmithRunePlatebody();
        static Activity* AlchRunePlatebody();
};

#endif