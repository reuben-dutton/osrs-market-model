#include <stdio.h>
#include <stdbool.h>
#include <cmath>
#include <vector>
#include <iostream>
#include <algorithm>
#include <string>

#include "strategies.h"


int activity_profit(Market &market, Activity* activity, int turnDuration) {
    int profit = 0;

    for (std::pair<std::string, int> requirement : activity->requirements) {
        std::string itemName = requirement.first;
        int itemCount = requirement.second;
        std::pair<int, int> margin = market.get_unit_prices(itemName);
        int price = std::max(margin.first, margin.second);
        if (!price) {
            return 1;
        }
        profit -= price*itemCount;
    }

    for (std::pair<std::string, int> product : activity->products) {
        std::string itemName = product.first;
        int itemCount = product.second;
        std::pair<int, int> margin = market.get_unit_prices(itemName);
        int price = std::max(margin.first, margin.second);
        if (!price) {
            return 1;
        }
        profit += price*itemCount;
    }

    profit -= activity->moneyInput;
    profit += activity->moneyOutput;

    profit = profit * ((float)turnDuration / (float)activity->duration);

    return profit;
}

PriceStrategy::PriceStrategy(int _impatience, int _profitMotive) {
    impatience = _impatience;
    profitMotive = _profitMotive;
}

int PriceStrategy::calculate_sell_price(Market &market, Activity* activity, int turnDuration, std::string itemName, int prevFailures) {
    std::pair<int, int> margin = market.get_unit_prices(itemName);
    int buyPrice = margin.first;
    int sellPrice = margin.second;

    int activityProfit = activity_profit(market, activity, turnDuration);
    int scaledActivityProfit = activityProfit / ((float)turnDuration / (float)activity->duration);

    int finalPrice;
    
    // initialize price
    if (!buyPrice && !sellPrice) {
        finalPrice = 10000;
    } else if (!sellPrice) {
        finalPrice = (int)buyPrice*1.01;
    } else {
        finalPrice = sellPrice;
    }

    // agents with more motivation to make profit will be more stringent when undercutting/overpricing
    // 0 profit motive -> 10% under/over, 100 profit motive -> 0.1% under/over
    double smallAdjTend = ((100 - profitMotive) * 0.01)*10 + 1;
    double largeAdjTend = ((100 - profitMotive) * 0.01) * (finalPrice * 0.001);
    int minPriceAdjustment = std::max((int)largeAdjTend, (int)smallAdjTend);
    // minPriceAdjustment = 10;
    double failureAdj = (minPriceAdjustment * (prevFailures + 1) * impatience * 0.0001);
    // minPriceAdjustment += (int)failureAdj;

    // maximum possible adjustment (in the case that the activity is unsuccessful) is the current profit (per unit)
    // adjusted by a factor corresponding to the agent's impatience (higher impatience, higher adjustment)
    int failurePriceAdjustment = std::max((int)(scaledActivityProfit * (impatience * 0.001)), minPriceAdjustment);
    failurePriceAdjustment = minPriceAdjustment;

    // maximum possible adjustment (in the case that the activity is successful) is a proportion of the
    // current price, adjusted by a factor corresponding to the agent's motivation to seek profit
    // (this maxes out at 1%, minimal is minPriceAdjustment)
    int successPriceAdjustment = std::max((int)(finalPrice * (profitMotive * 0.0001)), minPriceAdjustment);
    successPriceAdjustment = minPriceAdjustment;

    int minProfit = profitMotive * 0.01 * 2000;
    // is the activity profitable?
    bool profitable = (scaledActivityProfit - 1 > minProfit);
    bool successful = (prevFailures < 1);

    if (successful) {
        finalPrice += successPriceAdjustment;
    } else if (profitable) {
        // finalPrice -= failurePriceAdjustment * prevFailures; 
        finalPrice -= failurePriceAdjustment;
    } else {
        // finalPrice = buyPrice - minPriceAdjustment;
    }

    return std::max(finalPrice, 1);
}

int PriceStrategy::calculate_buy_price(Market &market, Activity* activity, int turnDuration, std::string itemName, int prevFailures) {
    std::pair<int, int> margin = market.get_unit_prices(itemName);
    int buyPrice = margin.first;
    int sellPrice = margin.second;

    int activityProfit = activity_profit(market, activity, turnDuration);
    int scaledActivityProfit = activityProfit / ((float)turnDuration / (float)activity->duration);

    int finalPrice;

    // initializing price
    if (!buyPrice && !sellPrice) {
        finalPrice = 10000;
    } else if (!buyPrice) {
        finalPrice = (int)sellPrice*0.99;
    } else {
        finalPrice = buyPrice;
    }

    // agents with more motivation to make profit will be more stringent when undercutting/overpricing
    // 0 profit motive -> 2% under/over, 100 profit motive -> 0.1% under/over
    double smallAdjTend = ((100 - profitMotive) * 0.01)*10 + 1;
    double largeAdjTend = ((100 - profitMotive) * 0.005) * (finalPrice * 0.001);
    int minPriceAdjustment = std::max((int)largeAdjTend, (int)smallAdjTend);
    // minPriceAdjustment = 10;
    double failureAdj = (minPriceAdjustment * (prevFailures + 1) * impatience * 0.001);
    // minPriceAdjustment += (int)failureAdj;

    // maximum possible adjustment (in the case that the activity is unsuccessful) is the current profit
    // adjusted by a factor corresponding to the agent's impatience (higher impatience, higher adjustment)
    int failurePriceAdjustment = std::max((int)(scaledActivityProfit * (impatience + 0.001)), minPriceAdjustment);
    failurePriceAdjustment = minPriceAdjustment;

    // maximum possible adjustment (in the case that the activity is successful) is a proportion of the
    // current price, adjusted by a factor corresponding to the agent's motivation to seek profit
    // (this maxes out at 1%, minimal is minPriceAdjustment)
    int successPriceAdjustment = std::max((int)(finalPrice * (profitMotive * 0.0001)), minPriceAdjustment);
    successPriceAdjustment = minPriceAdjustment;

    int minProfit = profitMotive * 0.01 * 2000;
    // is the activity profitable?
    bool profitable = (scaledActivityProfit - 1 > minProfit);
    // have our trade offers been successful?
    bool successful = (prevFailures < 1);

    if (successful) {
        finalPrice -= successPriceAdjustment;
    } else if (profitable) {
        // finalPrice += failurePriceAdjustment * prevFailures;
        finalPrice += failurePriceAdjustment;
    } else {
        // finalPrice = sellPrice + minPriceAdjustment;
    }

    return std::max(finalPrice, 1);
}