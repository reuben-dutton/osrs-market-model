#ifndef ACTIVITY_INCLUDED
#define ACTIVITY_INCLUDED

#include <stdbool.h>
#include <map>
#include <vector>
#include <string>

class Activity {
    public:
        std::string name;
        std::vector<std::string> requirements;
        std::vector<std::string> products;
        int moneyOutput;
        int moneyInput;

        Activity(std::string _name);
        void add_requirement(std::string itemName);
        void add_product(std::string itemName);
        void add_money_output(int _moneyOutput);
        void add_money_input(int _moneyInput);
        void print_activity();
        bool has_required_items(std::map<std::string, int> bank);
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