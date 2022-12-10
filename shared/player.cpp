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

    thread_local std::random_device rd;  //Will be used to obtain a seed for the random number engine
    thread_local std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> distrib(1, 100);

    int activityProfit = activity_profit(market, activity);

    bool caresAboutProfit = distrib(gen) < 95;

    // the worse the profit, the more likely not to perform that activity
    bool lazy = distrib(gen) < (80 - std::min(10000, activityProfit)*0.01);
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

    std::vector<std::pair<std::string, int>> needs = activity->requirements;

    // iterate over items in bank to sell
    // this should be made more intricate later
    auto it = bank.begin();
    while (it != bank.end()) {
        std::string bankItemName = it->first;
        int bankItemCount = it->second;

        bool itemRequired = false;

        // iterate over required items
        for (std::pair<std::string, int> need : needs) {
            std::string requireItemName = need.first;
            // our current bank item is a requirement
            if (bankItemName == requireItemName) {
                itemRequired = true;
            }
        }
        // if the item is not required for our activity
        if (!itemRequired && bankItemCount > 0) {
            // sell the entire stack
            this->create_sell_listing(market, bankItemName, bankItemCount);
        }
        it++;
    }

    // create listings to purchase activity requirements
    // for the next period
    if (doingActivity) {
        for (std::pair<std::string, int> need : needs) {
            std::string itemName = need.first;
            int itemCount = need.second;
            this->create_buy_listing(market, itemName, itemCount);
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
    for (std::pair<std::string, int> requirement : activity->requirements) {
        std::string itemName = requirement.first;
        int itemCount = requirement.second;
        this->remove_item(itemName, itemCount);
    }
    for (std::pair<std::string, int> product : activity->products) {
        std::string itemName = product.first;
        int itemCount = product.second;
        this->add_item(itemName, itemCount);
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