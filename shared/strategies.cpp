#include <stdio.h>
#include <stdbool.h>
#include <cmath>
#include <vector>
#include <iostream>
#include <algorithm>
#include <string>

#include "strategies.h"


int activity_profit(Market &market, Activity* activity) {
    int profit = 0;

    for (std::string itemName : activity->requirements) {
        std::pair<int, int> margin = market.get_unit_prices(itemName);
        int price = std::max(margin.first, margin.second);
        if (!price) {
            return 1;
        }
        profit -= price;
    }

    for (std::string itemName : activity->products) {
        std::pair<int, int> margin = market.get_unit_prices(itemName);
        int price = std::min(margin.first, margin.second);
        if (!price) {
            return 1;
        }
        profit += price;
    }

    profit -= activity->moneyInput;
    profit += activity->moneyOutput;
    return profit;
}

int DefaultPS::calculate_sell_price(Market &market, Activity* activity, std::string itemName, int prevFailures) {
    std::pair<int, int> margin = market.get_unit_prices(itemName);
    int buyPrice = margin.first;
    int sellPrice = margin.second;

    int activityProfit = activity_profit(market, activity);

    int finalPrice;
    
    // initialize price
    if (!buyPrice && !sellPrice) {
        finalPrice = 10000;
    } else if (!sellPrice) {
        finalPrice = (int)buyPrice*1.01;
    } else {
        finalPrice = sellPrice;
    }

    bool profitable = (activityProfit - SIMULATION_SPEED > 0);
    bool successful = (prevFailures < 2);

    if (successful) {
        finalPrice += SIMULATION_SPEED;
    } else if (profitable) {
        finalPrice -= SIMULATION_SPEED; 
    }

    return std::max(finalPrice, 1);
}

int DefaultPS::calculate_buy_price(Market &market, Activity* activity, std::string itemName, int prevFailures) {
    std::pair<int, int> margin = market.get_unit_prices(itemName);
    int buyPrice = margin.first;
    int sellPrice = margin.second;

    int activityProfit = activity_profit(market, activity);

    int finalPrice;

    // initializing price
    if (!buyPrice && !sellPrice) {
        finalPrice = 10000;
    } else if (!buyPrice) {
        finalPrice = (int)sellPrice*0.99;
    } else {
        finalPrice = buyPrice;
    }

    // is the activity profitable?
    bool profitable = (activityProfit - SIMULATION_SPEED > 0);
    // have our trade offers been successful?
    bool successful = (prevFailures < 2);

    if (successful) {
        finalPrice -= SIMULATION_SPEED;
    } else if (profitable) {
        finalPrice += SIMULATION_SPEED;
    }

    return std::max(finalPrice, 1);
}

int GreedyPS::calculate_sell_price(Market &market, Activity* activity, std::string itemName, int prevFailures) {
    std::pair<int, int> margin = market.get_unit_prices(itemName);
    int buyPrice = margin.first;
    int sellPrice = margin.second;

    int finalPrice;
    
    if (!buyPrice && !sellPrice) {
        finalPrice = 100;
    }

    if (!sellPrice) {
        finalPrice = (int)buyPrice*1.01;
    }

    finalPrice = sellPrice + 1 + (int)sellPrice*0.03;

    finalPrice -= (int)finalPrice*0.1*prevFailures;

    return std::max(finalPrice, 1);
}

int GreedyPS::calculate_buy_price(Market &market, Activity* activity, std::string itemName, int prevFailures) {
    std::pair<int, int> margin = market.get_unit_prices(itemName);
    int buyPrice = margin.first;
    int sellPrice = margin.second;

    int finalPrice;

    if (!buyPrice && !sellPrice) {
        finalPrice = 100;
    }

    if (!buyPrice) {
        finalPrice = (int)sellPrice*0.9;
    }

    finalPrice = buyPrice - 1 - (int)buyPrice*0.03;

    finalPrice += (int)finalPrice*0.1*prevFailures;

    return std::max(finalPrice, 1);
}

PriceStrategy* PriceStrategies::_default;

PriceStrategy* PriceStrategies::Default() {
    if (_default == NULL) {
        _default = new DefaultPS();
    }
    return _default;
}

PriceStrategy* PriceStrategies::_greedy;

PriceStrategy* PriceStrategies::Greedy() {
    if (_greedy == NULL) {
        _greedy = new GreedyPS();
    }
    return _greedy;
}