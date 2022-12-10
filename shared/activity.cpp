#include <stdio.h>
#include <stdbool.h>
#include <map>
#include <cmath>
#include <vector>
#include <iostream>
#include <string>

#include "activity.h"


Activity::Activity(std::string _name, int _duration) {
    name = _name;
    duration = _duration;
    moneyOutput = 0;
    moneyInput = 0;

    prevActiveDurationProducts = 0;
    prevActiveDurationRequirements = 0;

    requirements = {};
    products = {};
    activeRequirements = {};
    activeProducts = {};
}

void Activity::add_requirement(std::string itemName, int itemCount) {
    requirements.push_back(std::make_pair(itemName, itemCount));
}

void Activity::add_product(std::string itemName, int itemCount) {
    products.push_back(std::make_pair(itemName, itemCount));
}

void Activity::add_money_output(int _moneyOutput) {
    moneyOutput = _moneyOutput;
}

void Activity::add_money_input(int _moneyInput) {
    moneyInput = _moneyInput;
}

bool Activity::has_required_items(std::map<std::string, int> bank, int activeDuration) {
    int numInstances = (int) activeDuration / duration;

    bool has_items = true;
    for (std::pair<std::string, int> requirement : requirements) {
        std::string itemName = requirement.first;
        int itemCountRequired = requirement.second * numInstances;
        auto it = bank.find(itemName);
        if (it != bank.end()) {
            int itemCount = it->second;
            has_items = has_items && itemCount >= itemCountRequired;
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
        for (std::pair<std::string, int> requirement : requirements) {
            std::cout << requirement.first << " x" << requirement.second << "   ";
        }
        std::cout << std::endl;
    } else {
        std::cout << "None" << std::endl;
    }
    std::cout << "Products: ";
    if (products.size() > 0) {
        for (std::pair<std::string, int> product : products) {
            std::cout << product.first << " x" << product.second << "   ";
        }
        std::cout << std::endl;
    } else {
        std::cout << "None" << std::endl;
    }
}

ItemVector Activity::get_products(int activeDuration) {
    if (activeDuration == prevActiveDurationProducts) {
        return activeProducts;
    }

    int numInstances = (int) activeDuration / duration;
    activeProducts = {};

    for (std::pair<std::string, int> item : products) {
        std::string itemName = item.first;
        int itemCount = item.second;
        activeProducts.push_back(std::make_pair(itemName, numInstances*itemCount));
    }

    return activeProducts;
}

ItemVector Activity::get_requirements(int activeDuration) {
    if (activeDuration == prevActiveDurationRequirements) {
        return activeRequirements;
    }

    activeRequirements = {};
    for (std::pair<std::string, int> item : requirements) {
        std::string itemName = item.first;
        int itemCount = item.second;
        int numInstances = (int) activeDuration / duration;
        activeRequirements.push_back(std::make_pair(itemName, numInstances*itemCount));
    }

    return activeRequirements;
}


Activity* Activities::_mineRunite;

Activity* Activities::MineRunite() {
    if (_mineRunite == NULL) {
        _mineRunite = new Activity("Mine Runite", 40); // roughly 40 ticks each ore
        // runite also limited by respawn times?
        _mineRunite->add_product("Runite Ore", 1);
    }
    return _mineRunite;
}

Activity* Activities::_mineCoal;

Activity* Activities::MineCoal() {
    if (_mineCoal == NULL) {
        _mineCoal = new Activity("Mine Coal", 10); // roughly? guessing
        _mineCoal->add_product("Coal", 1);
    }
    return _mineCoal;
}

Activity* Activities::_smithRuniteBar;

Activity* Activities::SmithRuniteBar() {
    if (_smithRuniteBar == NULL) {
        _smithRuniteBar = new Activity("Smith Runite Bar", 4); // 4 ticks at a furnace
        _smithRuniteBar->add_requirement("Runite Ore", 1);
        _smithRuniteBar->add_requirement("Coal", 8);
        _smithRuniteBar->add_product("Runite Bar", 1);
    }
    return _smithRuniteBar;
}

Activity* Activities::_smithRunePlatebody;

Activity* Activities::SmithRunePlatebody() {
    if (_smithRunePlatebody == NULL) {
        _smithRunePlatebody = new Activity("Smith Rune Plateleg", 5); // 5 ticks at an anvil
        _smithRunePlatebody->add_requirement("Runite Bar", 3);
        _smithRunePlatebody->add_product("Rune Plateleg", 1);
    }
    return _smithRunePlatebody;
}

Activity* Activities::_alchRunePlatebody;

Activity* Activities::AlchRunePlatebody() {
    if (_alchRunePlatebody == NULL) {
        _alchRunePlatebody = new Activity("Alch Rune Plateleg", 5);
        _alchRunePlatebody->add_requirement("Nature Rune", 1);
        _alchRunePlatebody->add_requirement("Rune Plateleg", 1);
        _alchRunePlatebody->add_money_output(38400);
    }
    return _alchRunePlatebody;
}

Activity* Activities::_craftNatureRunes;

Activity* Activities::CraftNatureRunes() {
    if (_craftNatureRunes == NULL) {
        _craftNatureRunes = new Activity("Craft Nature Runes", 110);
        _craftNatureRunes->add_product("Nature Rune", 54);
    }
    return _craftNatureRunes;
}