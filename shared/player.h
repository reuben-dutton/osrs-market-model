#ifndef PLAYER_INCLUDED
#define PLAYER_INCLUDED

#include <vector>
#include <map>
#include <string>

#include "items.h"
#include "market.h"
#include "activity.h"
#include "strategies.h"


class Agent {
    public:
        int money;
        std::map<std::string, int> bank;
        Activity* activity;
        PriceStrategy* priceStrategy;

        std::vector<TradeID> trades;
        std::unordered_map<std::string, int> tradeFailures;

        Agent(int _turnDuration, int _impatience, int _profitMotive);
        void print_agent();
        void add_money(int _money);
        void remove_money(int _money);
        void add_item(std::string itemName, int itemCount);
        void remove_item(std::string itemName, int itemCount);
        void act(Market &market);
        void perform_activity();
        void create_listings(Market &market, bool doingActivity);
        void check_listings(Market &market);
        void remove_listing(Market &market, TradeID tradeID);
        void create_sell_listing(Market &market, std::string unitName, int unitCount);
        void create_buy_listing(Market &market, std::string unitName, int unitCount);
    
    private:
        int turnDuration;
        int impatience;
        int profitMotive;
};

#endif