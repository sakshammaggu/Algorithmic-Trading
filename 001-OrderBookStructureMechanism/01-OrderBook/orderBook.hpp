#ifndef ORDERBOOK_HPP
#define ORDERBOOK_HPP

#include <string>
#include <unordered_map>
#include <vector>

// ticker for the stock being traded
std::string TICKER = "GOOGL";

struct Balance {
    std::unordered_map<std::string, double> balance; // STORES USD : VALUE, GOOGL : VALUE

    Balance() {
        balance["USD"] = 0.0;
        balance[TICKER] = 0.0;
    }

    Balance(std::string stock, double value) {
        balance[stock] = value;
    }

    std::string addBalance(std::string stock, double value) {
        // if balance map contains the stock, add the value to the balance
        if (balance.find(stock) != balance.end()) {
            balance[stock] += value;
            return "Balance added successfully";
        }
        balance[stock] = value;
        return "Balance added successfully";
    }
};

struct User {
    std::string userName;
    Balance userBalance;

    User(){};

    User(std::string name, Balance b) {
        userName = name;
        userBalance = b;
    }

    User(std::string name) {
        userName = name;
        userBalance = Balance(); // default balance is 1000 USD and 0 GOOGL    
    }
};

struct Order {

};

class OrderBook {

};

#endif // ORDERBOOK_HPP