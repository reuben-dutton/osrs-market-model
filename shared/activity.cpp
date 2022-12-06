#include <stdio.h>
#include <stdbool.h>
#include <map>
#include <cmath>
#include <vector>
#include <iostream>
#include <string>

#include "activity.h"


Activity::Activity(std::string _name) {
    name = _name;
    moneyOutput = 0;
    moneyInput = 0;
}

void Activity::add_requirement(std::string itemName) {
    requirements.push_back(itemName);
}

void Activity::add_product(std::string itemName) {
    products.push_back(itemName);
}

void Activity::add_money_output(int _moneyOutput) {
    moneyOutput = _moneyOutput;
}

void Activity::add_money_input(int _moneyInput) {
    moneyInput = _moneyInput;
}

bool Activity::has_required_items(std::map<std::string, int> bank) {
    bool has_items = true;
    for (std::string itemName : requirements) {
        auto it = bank.find(itemName);
        if (it != bank.end()) {
            int itemCount = it->second;
            has_items = has_items && itemCount >= 1;
        } else {
            has_items = false;
        }
    }
    return has_items;
}

void Activity::print_activity() {
    std::cout << name << std::endl;
    std::cout << "Requirements: ";
    if (requirements.size() > 0) {
        for (std::string itemName : requirements) {
            std::cout << itemName << "   ";
        }
        std::cout << std::endl;
    } else {
        std::cout << "None" << std::endl;
    }
    std::cout << "Products: ";
    if (products.size() > 0) {
        for (std::string itemName : products) {
            std::cout << itemName << "   ";
        }
        std::cout << std::endl;
    } else {
        std::cout << "None" << std::endl;
    }
}

Activity* Activities::_mineRunite;

Activity* Activities::MineRunite() {
    if (_mineRunite == NULL) {
        _mineRunite = new Activity("Mine Runite");
        _mineRunite->add_product("Runite Ore");
    }
    return _mineRunite;
}

Activity* Activities::_mineCoal;

Activity* Activities::MineCoal() {
    if (_mineCoal == NULL) {
        _mineCoal = new Activity("Mine Coal");
        _mineCoal->add_product("Coal");
    }
    return _mineCoal;
}

Activity* Activities::_smithRuniteBar;

Activity* Activities::SmithRuniteBar() {
    if (_smithRuniteBar == NULL) {
        _smithRuniteBar = new Activity("Smith Runite Bar");
        _smithRuniteBar->add_requirement("Runite Ore");
        _smithRuniteBar->add_requirement("Coal");
        _smithRuniteBar->add_product("Runite Bar");
    }
    return _smithRuniteBar;
}

Activity* Activities::_smithRunePlatebody;

Activity* Activities::SmithRunePlatebody() {
    if (_smithRunePlatebody == NULL) {
        _smithRunePlatebody = new Activity("Smith Rune Platebody");
        _smithRunePlatebody->add_requirement("Runite Bar");
        _smithRunePlatebody->add_product("Rune Platebody");
    }
    return _smithRunePlatebody;
}

Activity* Activities::_alchRunePlatebody;

Activity* Activities::AlchRunePlatebody() {
    if (_alchRunePlatebody == NULL) {
        _alchRunePlatebody = new Activity("Alch Rune Platebody");
        _alchRunePlatebody->add_requirement("Rune Platebody");
        _alchRunePlatebody->add_money_output(39000);
    }
    return _alchRunePlatebody;
}