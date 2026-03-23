#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>

using namespace std;
/**
 * Exchange
 * --------
 * A simplified order matching engine for stock trading.
 *
 * Supports:
 * - Buy and sell limit orders
 * - Price-time priority matching (price first, then FIFO implicitly by structure)
 * - Multiple symbols (AAPL, TSLA, etc.)
 *
 * Data Structures:
 * - For each symbol:
 *      buy orders  → sorted in descending price (highest bid first)
 *      sell orders → sorted in ascending price (lowest ask first)
 *
 * Matching Rules:
 * - Buy order matches with lowest available sell price ≤ buy price
 * - Sell order matches with highest available buy price ≥ sell price
 *
 * If order is not fully matched, the remaining quantity is added to the book.
 */
class Exchange {
private:

    /**
     * Lists of symbols that currently have active orders.
     * Used for printing in sorted order.
     */
    vector<string> buy_tickers;
    vector<string> sell_tickers;

    /**
     * Represents the order book for a single stock.
     *
     * buy  → vector of (price, quantity), sorted descending
     * sell → vector of (price, quantity), sorted ascending
     */
    typedef struct Stock{
        vector<pair<int,int>> buy;
        vector<pair<int,int>> sell;
    } Stock;

    /**
     * Main storage: symbol → order book
     */
    unordered_map<string,Stock> stocks;

    /**
     * Removes a symbol from the ticker list.
     * Uses swap-with-last for O(1) removal.
     */
    void delete_stock(vector<string>& tickers,const string& symbol)
    {
        for(int i = 0; i < tickers.size(); i++){
            if(tickers[i] == symbol)
            {
                tickers[i] = tickers.back();
                tickers.pop_back();
                break;
            }
        }
    }

    /**
     * Adds symbol to ticker list if not already present.
     */
    void val_in_tickers(vector<string>& tickers , const string& symbol)
    {
        for(const auto& tic: tickers){
            if(tic == symbol) return;
        }
        tickers.push_back(symbol);
    }

    /**
     * Matches a BUY order against existing SELL orders.
     *
     * Strategy:
     * - Always match against the lowest sell price first
     * - Continue until:
     *      - No more matching prices
     *      - Quantity is exhausted
     *
     * Remaining quantity is inserted into the BUY book.
     */
    void match_buy(vector<pair<int,int>>& sell,int price, int quantity,const string& symbol)
    {
        // Ensure best price (lowest sell) is first
        sort(sell.begin(), sell.end());

        while(!sell.empty() && quantity > 0){
            // Check if prices cross
            if(sell.front().first <= price){
                if(quantity < sell.front().second){
                    sell.front().second -= quantity;
                    quantity = 0;
                }
                else{
                    quantity -= sell.front().second;
                    sell.erase(sell.begin());
                }
            }
            else break;
        }

        // Remaining quantity goes to BUY book
        if(quantity > 0){
            stocks[symbol].buy.emplace_back(price, quantity);
            sort(stocks[symbol].buy.rbegin(), stocks[symbol].buy.rend());
            val_in_tickers(buy_tickers,symbol);
        }

        // Clean up if no more sell orders
        if(sell.empty()) delete_stock(sell_tickers,symbol);
    }

    /**
     * Matches a SELL order against existing BUY orders.
     *
     * Strategy:
     * - Always match against the highest buy price first
     * - Continue until:
     *      - No more matching prices
     *      - Quantity is exhausted
     *
     * Remaining quantity is inserted into the SELL book.
     */
    void match_sell(vector<pair<int,int>>& buy,int price, int quantity,const string& symbol)
    {
        // Ensure best price (highest buy) is first
        sort(buy.rbegin(), buy.rend());

        while(!buy.empty() && quantity > 0){
            // Check if prices cross
            if(buy.front().first >= price){
                if(quantity < buy.front().second){
                    buy.front().second -= quantity;
                    quantity = 0;
                }
                else{
                    quantity -= buy.front().second;
                    buy.erase(buy.begin());
                }
            }
            else break;
        }

        // Remaining quantity goes to SELL book
        if(quantity > 0){
            stocks[symbol].sell.emplace_back(price, quantity);
            sort(stocks[symbol].sell.begin(),stocks[symbol].sell.end());
            val_in_tickers(sell_tickers,symbol);
        }

        // Clean up if no more buy orders
        if(buy.empty()) delete_stock(buy_tickers,symbol);
    }

public:

    /**
     * Adds a new order to the exchange.
     *
     * @param symbol   Stock symbol (e.g., "AAPL")
     * @param side     "buy" or "sell"
     * @param price    Limit price
     * @param quantity Order size
     *
     * Time Complexity:
     * - Matching: O(n log n) due to sorting
     */
    void addOrder(const string& symbol, string side, int price, int quantity)
    {
        Stock& stock = stocks[symbol];

        if(side == "sell") match_sell(stock.buy,price,quantity,symbol);
        else match_buy(stock.sell,price,quantity,symbol);
    }

    /**
     * Prints all remaining unmatched orders.
     *
     * Output format:
     * SYMBOL | PRICE | QUANTITY
     *
     * Buy orders printed first, then sell orders.
     */
    void printRemainingOrders()
    {
        sort(buy_tickers.begin(),buy_tickers.end());
        sort(sell_tickers.begin(),sell_tickers.end());

        cout<< "-------- BUY --------" << endl;
        for(const auto& tic : buy_tickers)
        {
            auto& pq = stocks[tic].buy;
            for(const auto& pair :pq)
            {
                cout << tic <<" | "<<pair.first <<" | "<< pair.second << endl;
            }
        }

        cout<< "-------- SELL --------" << endl;
        for(const auto& tic : sell_tickers)
        {
            auto& pq = stocks[tic].sell;
            for(const auto& pair :pq)
            {
                cout << tic <<" | "<<pair.first <<" | "<< pair.second << endl;
            }
        }
    }
};

int main() {
    Exchange ex;

    // ===== AAPL =====
    ex.addOrder("AAPL", "buy", 100, 10);
    ex.addOrder("AAPL", "buy", 101, 5);

    ex.addOrder("AAPL", "sell", 102, 7);
    ex.addOrder("AAPL", "sell", 100, 8);

    // ===== TSLA =====
    ex.addOrder("TSLA", "sell", 200, 10);
    ex.addOrder("TSLA", "buy", 210, 4);
    ex.addOrder("TSLA", "buy", 200, 10);

    // ===== GOOG =====
    ex.addOrder("GOOG", "buy", 50, 5);
    ex.addOrder("GOOG", "sell", 60, 5);

    // ===== FIFO test =====
    ex.addOrder("MSFT", "sell", 300, 5);
    ex.addOrder("MSFT", "sell", 300, 5);
    ex.addOrder("MSFT", "buy", 300, 7);


    cout << "Remaining Orders:\n";
    ex.printRemainingOrders();

    return 0;
}