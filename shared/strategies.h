#ifndef STRATEGIES_INCLUDED
#define STRATEGIES_INCLUDED

#ifndef SIMULATION_SPEED
#define SIMULATION_SPEED 100
#endif

#include <string>

#include "market.h"
#include "activity.h"

class PriceStrategy {
    public:
        virtual int calculate_sell_price(Market &market, Activity* activity, std::string itemName, int prevFailures) = 0;
        virtual int calculate_buy_price(Market &market, Activity* activity, std::string itemName, int prevFailures) = 0;
};

class DefaultPS : public PriceStrategy {
    public:
        int calculate_sell_price(Market &market, Activity* activity, std::string itemName, int prevFailures);
        int calculate_buy_price(Market &market, Activity* activity, std::string itemName, int prevFailures);
};

class GreedyPS : public PriceStrategy {
    public:
        int calculate_sell_price(Market &market, Activity* activity, std::string itemName, int prevFailures);
        int calculate_buy_price(Market &market, Activity* activity, std::string itemName, int prevFailures);
};

class PriceStrategies {
    private:
        static PriceStrategy* _default;
        static PriceStrategy* _greedy;

    public:
        static PriceStrategy* Default();
        static PriceStrategy* Greedy();
};

int activity_profit(Market &market, Activity* activity);

#endif