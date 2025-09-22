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

    std::string addBalances(std::string stock, double value) {
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
    std::string userName;
    std::string orderType; // BID or ASK
    double price;
    int quantity;
    
    static double orderCounterBid; // Declare a static counter to keep track of the order number
    static double orderCounterAsk; // Declare a static counter to keep track of the order number
    int insertionOrderBid; // To keep track of the insertion order for BID orders. Order in which the order was inserted into the order book
    int insertionOrderAsk; // To keep track of the insertion order for ASK orders. Order in which the order was inserted into the order book

    Order(std::string user, std::string type, double p, int q) {
        userName = user;
        orderType = type;
        price = p;
        quantity = q;
        if (orderType == "BID") {
            insertionOrderBid = orderCounterBid++; // Increment the counter and assign it to the order
        } else if (orderType == "ASK") {
            insertionOrderAsk = orderCounterAsk++; // Increment the counter and assign it to the order
        }
    }
};

class OrderBook {
    private:
    std::vector<Order> bids; // stores all the BID orders
    std::vector<Order> asks; // stores all the ASK orders
    std::unordered_map<std::string, User> users; // stores all the users
    void flipBalance(const std::string& userId1, const std::string& userId2, double quantity, double price);

    public:
    OrderBook() {};
    ~OrderBook() {};

    std::string addBid(std::string Username, int Price, int Quantity); // adds a bid or ask to the order book
    std::string addAsk(std::string Username, int Price, int Quantity); // adds a bid or ask to the order book
    void cancelBid(std::string Username, int Price, int Quantity); // cancels a bid or ask from the order book
    void cancelAsk(std::string Username, int Price, int Quantity); // cancels a bid or ask from the order book
    std::string getBalance(std::string username); // returns the balance of a user
    std::string getQuote(int qty); // returns the best bid and ask prices and quantities
    std::string getDepth(); // returns the entire order book and shows all bids and asks
    std::string makeUser(std::string); // creates a new user for people trying to join the market
    std::string addBalance(std::string Username, std::string market, int value); // adds balance to a user
};

#endif // ORDERBOOK_HPP