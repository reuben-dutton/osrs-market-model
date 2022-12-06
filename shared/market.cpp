#include <stdio.h>
#include <string.h>
#include <iostream>
#include <stdbool.h>
#include <cmath>
#include <string>

#include "market.h"
#include "items.h"


Trade::Trade(bool _isSelling, int _unitPrice, std::string _unitName, int _unitCount) {
    isSelling = _isSelling;
    unitPrice = _unitPrice;
    unitName = _unitName;
    unitCount = _unitCount;
    unitProgressCount = 0;
    unitProgressFiat = 0;
    status = TradeStatus::INITIALIZED;
}

bool Trade::is_null() {
    return status == TradeStatus::NULL_TRADE;
}

bool Trade::is_ongoing() {
    return status == TradeStatus::ONGOING;
}

bool Trade::is_selling() {
    return isSelling;
}

std::string Trade::get_unit_name() {
    return unitName;
}

// return the number of units left in the order
int Trade::units_left() {
    return unitCount - unitProgressCount;
}

bool Trade::is_valid_exchange(Trade &other) {
    bool paired = other.isSelling != isSelling;
    bool bothOngoing = other.is_ongoing() && this->is_ongoing();
    bool sameUnit = other.unitName == unitName;
    bool pricesMatch;
    // other price needs to be lower if current instance is buying
    if (other.is_selling()) {
        pricesMatch = unitPrice >= other.unitPrice;
    } else { // or higher if current instance is selling
        pricesMatch = unitPrice <= other.unitPrice;
    }
    return paired && bothOngoing && sameUnit && pricesMatch;
}

void Trade::update_status() {
    if (unitCount == unitProgressCount) {
        status = TradeStatus::SUCCESS;
    } else {
        status = TradeStatus::ONGOING;
    }
}

void Trade::cancel() {
    status = TradeStatus::CANCELLED;
}

// assumed that this is a valid exchange
// the instance Trade can be either buy or sell
void Trade::fill_exchange(Trade &other) {
    // can either fully fill the buy trade, or fully fill the sell trade
    int amountExchangeable = std::min(other.units_left(), this->units_left());

    // increment both trades by the amount exchanged
    other.unitProgressCount += amountExchangeable;
    unitProgressCount += amountExchangeable;

    // increment the profit/cost by (the amount) * (the higher other trade price)
    unitProgressFiat += amountExchangeable*other.unitPrice;
    other.unitProgressFiat += amountExchangeable*other.unitPrice;
}

void Trade::print_trade() {
    if (status == TradeStatus::NULL_TRADE) {
        std::cout << "NULL TRADE" << std::endl;
        return;
    }
    std::cout << unitName << " x";
    std::cout << unitCount << " @";
    std::cout << unitPrice << "gp | ";
    if (isSelling) {
        std::cout << "sold ";
    } else {
        std::cout << "bought ";
    }
    std::cout << unitProgressCount << " for ";
    std::cout << unitProgressFiat << "gp total || ";
    if (status == TradeStatus::ONGOING) {
        std::cout << "ongoing";
    } else if (status == TradeStatus::SUCCESS) {
        std::cout << "success";
    } else if (status == TradeStatus::CANCELLED) {
        std::cout << "cancelled";
    }
    std::cout << std::endl;
}

TradeID::TradeID(bool _isSelling, std::string _unitName, std::pair<int, int> _tradeMapKey) {
    isSelling = _isSelling;
    unitName = _unitName;
    tradeMapKey = _tradeMapKey;
}

Market::Market() {
    buyRegister = {};
    sellRegister = {};
    margins = {};
    nullTrade = new Trade(false, 0, "NULL", 0);
    nullTrade->status = TradeStatus::NULL_TRADE;
    currentTradeID = 0;
}

void Market::print_market(bool onlyMargin) {
    if (!onlyMargin) {
        std::cout << "--buy--" << std::endl;
        auto it = buyRegister.begin();
        while (it != buyRegister.end()) {
            TradeMap currentTrades = it->second;
            auto it2 = currentTrades.begin();
            while (it2 != currentTrades.end()) {
                Trade trade = it2->second;
                trade.print_trade();
                it2++;
            }
            it++;
        }
        std::cout << "--sell--" << std::endl;
        it = sellRegister.begin();
        while (it != sellRegister.end()) {
            TradeMap currentTrades = it->second;
            auto it2 = currentTrades.begin();
            while (it2 != currentTrades.end()) {
                Trade trade = it2->second;
                trade.print_trade();
                it2++;
            }
            it++;
        }
    }
    std::cout << "--margin--" << std::endl;
    auto itm = margins.begin();
    while (itm != margins.end()) {
        std::cout << itm->first << ": ";
        std::cout << (itm->second).first << "gp ";
        std::cout << (itm->second).second << "gp ";
        std::cout << std::endl;
        itm++;
    }
}

int Market::get_trade_id() {
    return currentTradeID++;
}

TradeMap &Market::get_buy_trades(std::string unitName) {
    TradeMap emptyTradeMap = {};
    auto result = buyRegister.emplace(unitName, emptyTradeMap);
    // '.' to get the first in pair
    auto it = result.first;
    // -> to get the second in iterator pair
    TradeMap &currentTrades = it->second;
    return currentTrades;
}

TradeMap &Market::get_sell_trades(std::string unitName) {
    TradeMap emptyTradeMap = {};
    auto result = sellRegister.emplace(unitName, emptyTradeMap);
    // '.' to get the first in pair
    auto it = result.first;
    // -> to get the second in iterator pair
    TradeMap &currentTrades = it->second;
    return currentTrades;
}

TradeID Market::create_buy_trade(int unitPrice, std::string unitName, int unitCount) {
    Trade newTrade = Trade(false, unitPrice, unitName, unitCount);
    newTrade.status = TradeStatus::ONGOING;

    this->fill_new_trade(newTrade);

    TradeMap &currentTrades = this->get_buy_trades(unitName);
    

    // construct the key pair for the trade
    // unit price first to be ordered by that, new trade id second (also ordered)
    std::pair<int, int> uidPair = std::pair<int, int>(unitPrice, this->get_trade_id());
    currentTrades.insert({uidPair, newTrade});

    this->update_unit_prices(unitName);

    TradeID newTradeID = TradeID(false, unitName, uidPair);
    return newTradeID;
}

TradeID Market::create_sell_trade(int unitPrice, std::string unitName, int unitCount) {
    Trade newTrade = Trade(true, unitPrice, unitName, unitCount);
    newTrade.status = TradeStatus::ONGOING;

    this->fill_new_trade(newTrade);

    TradeMap &currentTrades = this->get_sell_trades(unitName);

    // construct the key pair for the trade
    // unit price first to be ordered by that, new trade id second (also ordered)
    std::pair<int, int> uidPair = std::pair<int, int>(unitPrice, this->get_trade_id());
    currentTrades.emplace(uidPair, newTrade);

    this->update_unit_prices(unitName);

    TradeID newTradeID = TradeID(true, unitName, uidPair);
    return newTradeID;
}

void Market::fill_new_trade(Trade &trade) {
    std::string unitName = trade.get_unit_name();
    if (trade.is_selling()) {
        TradeMap &currentTrades = this->get_buy_trades(unitName);
        // largest buy price to smallest,
        // we return the first hit that has a buy price larger
        // than the given sell price
        auto it = currentTrades.rbegin();
        while (it != currentTrades.rend()) {
            Trade &other = it->second;
            if (trade.is_valid_exchange(other)) {
                trade.fill_exchange(other);
                trade.update_status();
                other.update_status();
            }
            if (!trade.is_ongoing()) {
                break;
            }
            it++;
        }
    } else {
        TradeMap &currentTrades = this->get_sell_trades(unitName);
        // smallest sell price to largest,
        // we return the first hit that has a sell price smaller
        // than the given buy price
        auto it = currentTrades.begin();
        while (it != currentTrades.end()) {
            Trade &other = it->second;
            if (trade.is_valid_exchange(other)) {
                trade.fill_exchange(other);
                trade.update_status();
                other.update_status();
            }
            if (!trade.is_ongoing()) {
                break;
            }
            it++;
        }
    }
}

// Returns the trade specified by the given trade identifier
// If no trade exists, then returns NULL.
Trade &Market::check_trade(TradeID tradeID) {
    if (tradeID.isSelling) {
        try {
            TradeMap &currentTrades = sellRegister.at(tradeID.unitName);
            try {
                return currentTrades.at(tradeID.tradeMapKey);
            } catch (const std::out_of_range &exc) {
                return *nullTrade;
            }
        } catch (const std::out_of_range &exc) {
            return *nullTrade;
        } 
    } else {
        try {
            TradeMap &currentTrades = buyRegister.at(tradeID.unitName);
            try {
                return currentTrades.at(tradeID.tradeMapKey);
            } catch (const std::out_of_range &exc) {
                return *nullTrade;
            }
        } catch (const std::out_of_range &exc) {
            return *nullTrade;
        }
    }
    return *nullTrade;
}


Trade Market::cancel_trade(TradeID tradeID) {
    if (tradeID.isSelling) {
        try {
            TradeMap &currentTrades = sellRegister.at(tradeID.unitName);
            try {
                Trade returnTrade = currentTrades.at(tradeID.tradeMapKey);
                auto it = currentTrades.find(tradeID.tradeMapKey);
                currentTrades.erase(it);
                returnTrade.status = TradeStatus::CANCELLED;
                return returnTrade;
            } catch (const std::out_of_range &exc) {
                return *nullTrade;
            }
        } catch (const std::out_of_range &exc) {
            return *nullTrade;
        } 
    } else {
        try {
            TradeMap &currentTrades = buyRegister.at(tradeID.unitName);
            try {
                Trade returnTrade = currentTrades.at(tradeID.tradeMapKey);
                auto it = currentTrades.find(tradeID.tradeMapKey);
                currentTrades.erase(it);
                returnTrade.status = TradeStatus::CANCELLED;
                return returnTrade;
            } catch (const std::out_of_range &exc) {
                return *nullTrade;
            }
        } catch (const std::out_of_range &exc) {
            return *nullTrade;
        }
    }
    return *nullTrade;
}


void Market::update_unit_prices(std::string unitName) {

    int buyPrice = 0; // buy price margin (low)
    int sellPrice = 0; // sell price margin (high)

    auto itm = margins.find(unitName);
    if (itm != margins.end()) {
        buyPrice = (itm->second).first;
        sellPrice = (itm->second).second;
    }

    TradeMap &currentBuyTrades = this->get_buy_trades(unitName);
    // from highest buy price to lowest
    auto it1 = currentBuyTrades.rbegin();
    while (it1 != currentBuyTrades.rend()) {
        Trade &trade = it1->second;
        // don't care about status of trade
        if (true) {
            buyPrice = trade.unitPrice;
            break;
        }

        it1++;
    }

    TradeMap &currentSellTrades = this->get_sell_trades(unitName);
    // move from lowest sell price to highest
    auto it2 = currentSellTrades.begin();
    while (it2 != currentSellTrades.end()) {
        Trade &trade = it2->second;
        // don't care about status of trade
        if (true) {
            // this is our lowest sell price listed
            sellPrice = trade.unitPrice;
            break;
        }
        it2++;
    }
    std::pair<int, int> margin = {buyPrice, sellPrice};

    margins.erase(unitName);
    margins.emplace(unitName, margin);
}

std::pair<int, int> Market::get_unit_prices(std::string unitName) {
    try {
        return margins.at(unitName);
    } catch (const std::out_of_range &exc) {
        return std::make_pair(0, 0);
    }
}

void Market::print_unit_prices(std::string unitName) {
    std::pair<int, int> margin = margins.at(unitName);
    std::cout << unitName << " | ";
    std::cout << "buy/low: " << margin.first << "gp ";
    std::cout << "sell/high: " << margin.second << "gp ";
    std::cout << std::endl;
}