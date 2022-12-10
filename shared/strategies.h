#ifndef STRATEGIES_INCLUDED
#define STRATEGIES_INCLUDED

#include <string>

#include "market.h"
#include "activity.h"

class PriceStrategy {
    public:
        PriceStrategy(int _impatience, int _profitMotive);
        int calculate_sell_price(Market &market, Activity* activity, int turnDuration, std::string itemName, int prevFailures);
        int calculate_buy_price(Market &market, Activity* activity, int turnDuration, std::string itemName, int prevFailures);

    protected:
        int impatience;
        int profitMotive;
};

int activity_profit(Market &market, Activity* activity, int turnDuration);

#endif