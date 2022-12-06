#include <stdio.h>
#include <stdbool.h>
#include <cmath>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <random>
#include <string>

#include "market.h"
#include "player.h"

Agent::Agent() {
    money = 0;
    bank = {};
    trades = {};
    tradeFailures = {};
}

void Agent::print_agent() {
    std::cout << activity->name << std::endl;
    std::cout << money << "gp " << std::endl;
    auto it = bank.begin();
    while (it != bank.end()) {
        std::string itemName = it->first;
        int itemCount = it->second;
        std::cout << itemName << " x" << itemCount << std::endl;
        it++;
    }
    std::cout << std::endl;
}

void Agent::add_money(int _money) {
    money += _money;
}

void Agent::remove_money(int _money) {
    money -= _money;
}

void Agent::add_item(std::string itemName, int itemCount) {
    if (bank.find(itemName) == bank.end()) {
        bank[itemName] = 0;
    }
    bank[itemName] += itemCount;
}

void Agent::remove_item(std::string itemName, int itemCount) {
    if (bank.find(itemName) == bank.end()) {
        bank[itemName] = 0;
    }
    bank[itemName] -= itemCount;
    // bank[itemName] = std::max(bank[itemName], 0);
}

void Agent::act(Market &market) {

    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> distrib(1, 100);

    int activityProfit = activity_profit(market, activity);

    bool caresAboutProfit = distrib(gen) < 95;

    // the worse the profit, the more likely not to perform that activity
    bool lazy = distrib(gen) < (100 - std::min(10000, activityProfit)*0.01);
    // lazy = false;

    // activity switching // /// /// // // / // // // // /

    // bool adventurous = distrib(gen) < 40;

    // int smithProfit = activity_profit(market, Activities::SmithRunePlatebody());
    // int alchProfit = activity_profit(market, Activities::AlchRunePlatebody());

    // if (caresAboutProfit) {
    //     if (smithProfit < alchProfit || adventurous) {
    //         this->activity = Activities::AlchRunePlatebody();
    //     } else if (smithProfit > alchProfit || adventurous) {
    //         this->activity = Activities::SmithRunePlatebody();
    //     }
    // }

    bool doingActivity = false;
    this->check_listings(market);
    // if not lazy and profit above 0, or doesn't care about profit
    if ((!lazy && activityProfit > 0) || !caresAboutProfit) {
        doingActivity = true;
        if (activity->has_required_items(bank)) {
            this->perform_activity();
        }
    }
    this->create_listings(market, doingActivity);
}

void Agent::create_listings(Market &market, bool doingActivity) {

    std::vector<std::string> needs = activity->requirements;

    // iterate over items in bank
    auto it = bank.begin();
    while (it != bank.end()) {
        std::string itemName = it->first;
        // if the item is not a requirement for our activity,
        if (std::find(needs.begin(), needs.end(), itemName) == needs.end()) {
            int itemCount = it->second;
            if (!itemCount) {
                it++;
                continue;
            }
            // sell the entire stack
            this->create_sell_listing(market, itemName, itemCount);
        }
        it++;
    }

    // create listings to purchase activity requirements
    // for the next period
    if (doingActivity) {
        for (std::string itemName : needs) {
            this->create_buy_listing(market, itemName, 1);
        }
    }
}

void Agent::check_listings(Market &market) {
    auto it = trades.begin();
    while (it != trades.end()) {
        TradeID tradeID = *it;
        Trade trade = market.check_trade(tradeID);
        std::string itemName = trade.get_unit_name();

        if (trade.status == TradeStatus::ONGOING) {
            // remove the trade
            this->remove_listing(market, tradeID);
            it = trades.erase(it);

            if (trade.unitProgressCount > 0) {
                tradeFailures[itemName] = 0;
            } else {
                tradeFailures[itemName]++;
            }
            
            // these items will be relisted next cycle
        } else if (trade.status == TradeStatus::SUCCESS) {
            this->remove_listing(market, tradeID);
            it = trades.erase(it);
            tradeFailures[itemName] = 0;
        } else {
            it++;
        }
    }
}

void Agent::remove_listing(Market &market, TradeID tradeID) {
    // remove the listing
    Trade removed = market.cancel_trade(tradeID);

    // resolve the trade with the agent's bank
    if (removed.is_selling()) {
        // get the units not yet sold and the money received
        int unitsNotSold = removed.unitCount - removed.unitProgressCount;
        int fiatReceived = removed.unitProgressFiat;
        this->add_item(removed.get_unit_name(), unitsNotSold);
        this->add_money(fiatReceived);
    } else {
        // get the money not yet paid and the units bought
        int unitsBought = removed.unitProgressCount;
        int fiatNotUsed = (removed.unitCount * removed.unitPrice) - removed.unitProgressFiat;
        this->add_item(removed.get_unit_name(), unitsBought);
        this->add_money(fiatNotUsed);
    }
}

void Agent::perform_activity() {
    for (std::string itemName : activity->requirements) {
        this->remove_item(itemName, 1);
    }
    for (std::string itemName : activity->products) {
        this->add_item(itemName, 1);
    }
}

void Agent::create_sell_listing(Market &market, std::string unitName, int unitCount) {
    // calculate price
    // int price = 100;
    int price = this->priceStrategy->calculate_sell_price(market, this->activity, unitName, tradeFailures[unitName]);

    // use price to create trade
    TradeID newTradeID = market.create_sell_trade(price, unitName, unitCount);
    trades.push_back(newTradeID);

    // remove item from agent
    this->remove_item(unitName, unitCount);
}

void Agent::create_buy_listing(Market &market, std::string unitName, int unitCount) {
    // get price
    // int price = 100;
    int price = this->priceStrategy->calculate_buy_price(market, this->activity, unitName, tradeFailures[unitName]);

    // use price to create trade
    TradeID newTradeID = market.create_buy_trade(price, unitName, unitCount);
    trades.push_back(newTradeID);

    // remove money from agent
    this->remove_money(price*unitCount);
}