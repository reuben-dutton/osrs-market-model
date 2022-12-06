#ifndef PLAYER_INCLUDED
#define PLAYER_INCLUDED

#include <vector>
#include <map>

#include "items.h"
#include "market.h"
#include "activity.h"
#include "strategies.h"


class Agent {
    public:
        int money;
        std::map<const char*, int> bank;
        Activity* activity;
        PriceStrategy* priceStrategy;

        std::vector<TradeID> trades;
        std::unordered_map<const char*, int> tradeFailures;

        Agent();
        void print_agent();
        void add_money(int _money);
        void remove_money(int _money);
        void add_item(const char* itemName, int itemCount);
        void remove_item(const char* itemName, int itemCount);
        void act(Market &market);
        void perform_activity();
        void create_listings(Market &market, bool doingActivity);
        void check_listings(Market &market);
        void remove_listing(Market &market, TradeID tradeID);
        void create_sell_listing(Market &market, const char* unitName, int unitCount);
        void create_buy_listing(Market &market, const char* unitName, int unitCount);
};

#endif