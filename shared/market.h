#ifndef MARKET_INCLUDED
#define MARKET_INCLUDED

#include <vector>
#include <unordered_map>
#include <string>

#include "items.h"

typedef enum class TradeStatus {
    NULL_TRADE = -3,
    INITIALIZED = -1,
    ONGOING = 0,
    PARTIAL = 1,
    SUCCESS = 2,
    CANCELLED = 3
} TradeStatus;

class Trade {
    public:
        int unitPrice; // price of each item
        int unitCount; // number of items being sold/bought
        int unitProgressCount; // amount of items that have been sold/bought so far
        int unitProgressFiat; // actual total cost of units sold/bought so far
        TradeStatus status; // current status of the trade

        Trade(bool _isSelling, int _unitPrice, std::string _unitName, int _unitCount);
        bool is_null();
        bool is_ongoing();
        bool is_selling();
        std::string get_unit_name();
        int units_left();
        void update_status();
        void cancel();
        bool is_valid_exchange(Trade &other);
        void fill_exchange(Trade &other);
        void print_trade();
    
    protected:
        bool isSelling; // selling or buying
        std::string unitName; // item name
};

class TradeID {
    public:
        bool isSelling;
        std::string unitName;
        std::pair<int, int> tradeMapKey;

        TradeID(bool _isSelling, std::string _unitName, std::pair<int, int> _tradeMapKey);
};

// trade map has keys of <unitPrice, uid> so they are ordered by price
// values are Trade classes which contain trade information & status
typedef std::map<std::pair<int, int>, Trade> TradeMap;
// market register is unordered; keys are item names
typedef std::unordered_map<std::string, TradeMap> MarketRegister;
// recording the avg prices of items
typedef std::unordered_map<std::string, std::pair<int, int>> PriceRecord;

// represents the GE
class Market {
    public:
        MarketRegister buyRegister; // buy trades
        MarketRegister sellRegister; // sell trades

        PriceRecord margins; // average item prices
        
        Trade* nullTrade;
        
        Market();
        void print_market(bool onlyMargin);
        TradeMap &get_buy_trades(std::string unitName);
        TradeMap &get_sell_trades(std::string unitName);
        TradeID create_buy_trade(int unitPrice, std::string unitName, int unitCount);
        TradeID create_sell_trade(int unitPrice, std::string unitName, int unitCount);
        void fill_new_trade(Trade &trade);
        Trade &check_trade(TradeID tradeID);
        Trade cancel_trade(TradeID tradeID);

        void update_unit_prices(std::string unitName);
        std::pair<int, int> get_unit_prices(std::string unitName);
        void print_unit_prices(std::string unitName);

    private:
        int currentTradeID;
        int get_trade_id();
};

#endif