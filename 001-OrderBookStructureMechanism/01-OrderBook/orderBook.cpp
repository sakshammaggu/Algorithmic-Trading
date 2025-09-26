#include "orderBook.hpp"
#include <iostream>
#include <algorithm>
#include <iomanip>

using namespace std;

int Order::orderCounterBid = 0; // Initialize the static counter for BID orders
int Order::orderCounterAsk = 0; // Initialize the static counter for ASK orders

void OrderBook::flipBalance(const std::string& userId1, const std::string& userId2, double quantity, double price) {
    if (users.find(userId1) != users.end() && users.find(userId2) != users.end()) {
        if (users[userId1].userBalance.balance["USD"] >= quantity * price) {
            if (users[userId2].userBalance.balance[TICKER] >= quantity) {
                users[userId1].userBalance.balance["USD"] -= quantity * price;
                users[userId1].userBalance.balance[TICKER] += quantity;

                users[userId2].userBalance.balance["USD"] += quantity * price;
                users[userId2].userBalance.balance[TICKER] -= quantity;

                cout << "Funds and stocks Transaction successful: " << userId1 << " bought " << quantity << " " << TICKER << " from " << userId2 << " at price " << price << endl;
                return;
            } else {
                cout << "User " << userId2 << " does not have enough " << TICKER << " balance to complete the transaction." << endl;
                return;
            }
        } else {
            cout << "User " << userId1 << " does not have enough USD balance to complete the transaction." << endl;
            return;
        }
    } else {
        cout << "One or both users not found." << endl;
        return;
    }
}

// Implementation of OrderBook constructor
OrderBook::OrderBook() {
    // User 1 with initial balance
    Balance balance1("USD", 10000.0);
    balance1.addBalances(TICKER, 1000.0);
    User user1("MarketMaker1", balance1);
    users[user1.userName] = user1;

    cout << "Initialized OrderBook with Market Makers 1 and balance 1." << endl;

    Order bid1(user1.userName, "BID", 110.0, 10);
    Order ask1(user1.userName, "ASK", 115.0, 5);
    Order bid2(user1.userName, "BID", 111.0, 8);
    Order ask2(user1.userName, "ASK", 119.0, 12);

    bids.push_back(bid1);
    asks.push_back(ask1);
    bids.push_back(bid2);
    asks.push_back(ask2);

    // User 2 with initial balance
    Balance balance2("USD", 10000.0);
    balance2.addBalances(TICKER, 2000.0);
    User user2("MarketMaker2", balance2);
    users[user2.userName] = user2;

    cout << "Initialized OrderBook with Market Makers 2 and balance 2." << endl;

    Order bid3(user2.userName, "bid", 109.0, 10);
    Order ask3(user2.userName, "ask", 125.0, 5);
    Order bid4(user2.userName, "bid", 112.0, 8);
    Order ask4(user2.userName, "ask", 120.0, 12);

    bids.push_back(bid3);
    asks.push_back(ask3);
    bids.push_back(bid4);
    asks.push_back(ask4);

    // User 3 with initial balance
    Balance balance3("USD", 50000.0);
    balance3.addBalances(TICKER, 0.0);
    User user3("MarketMaker3", balance3);
    users[user3.userName] = user3;

    cout << "Initialized OrderBook with Market Makers 3 and balance 3." << endl;

    Order bid5(user3.userName, "bid", 105, 10);
    Order bid6(user3.userName, "bid", 108, 10);

    bids.push_back(bid5);
    bids.push_back(bid6);
}

std::string OrderBook::makeUser(std::string username) {
    User newUser(username);
    if (users.find(username) == users.end()) {
        users[username] = newUser;
        return "User " + username + " created successfully.";
    } 
    return "User " + username + " already exists.";
}

std::string OrderBook::addBid(std::string Username, double Price, int Quantity) {
    /*
        Implementation of adding a BID order to the order book.

        We need to check if the username exists in the users array, then we compare the value of the bid with the lowest ask price, if bid is higher or equal to ask then we start flipping balances and traversing through ask array and bid quantity till condition is false, then if remQty > 0
        we add the remaining quantity to the bids array else we return a message saying Bid Satisfied Successfully
    */

    // First check if the username exists in the users map
    if (users.find(Username) == users.end()) {
        return "Error: User " + Username + " does not exist.";
    }

    // Check if user has enough USD balance for the bid
    if (users[Username].userBalance.balance["USD"] < Price * Quantity) {
        return "Error: User " + Username + " does not have enough USD balance for this bid.";
    }

    int remQty = Quantity; // remaining quantity to be fulfilled

    stable_sort(asks.begin(), asks.end(), [](const Order &a, const Order &b) {
        // If prices are equal, maintain the original order
        if (a.price == b.price) {
            return a.insertionOrderAsk < b.insertionOrderAsk; // Earlier insertion order has higher priority
        }
        return a.price < b.price; // sort by price, Lower price has higher priority
    });

    for (auto it = asks.begin(); it != asks.end();) {
        if (remQty > 0  && Price >= it->price) {
            if (it->quantity > remQty) {
                it->quantity -= remQty;
                flipBalance(Username, it->userName, remQty, it->price);
                cout << "Bid Satisfied Successfully at price: " << it->price << " and quantity: " << remQty << endl;
                remQty = 0;
                break;
            } else {
                remQty -= it->quantity;
                flipBalance(Username, it->userName, it->quantity, it->price);
                cout << "Bid Satisfied Partially at price: " << it->price << " and quantity: " << it->quantity << endl;
                it = asks.erase(it); // Remove the ask order as it is completely fulfilled
            }
        } else {
            ++it;
        }
    }

    if (remQty > 0) {
        Order bid(Username, "bid", Price, remQty);
        bids.push_back(bid);
        cout << "Remaining quantity of bids added to Orderbook" << endl;
    }

    if (remQty == 0) {
        cout << "Complete Bid Satisfied Successfully" << endl;
    }

    return "Bid added/satified successfully.";
    
}

std::string OrderBook::addAsk(std::string Username, double Price, int Quantity) {
    
}

void OrderBook::cancelBid(std::string Username, double Price, int Quantity) {
    
}

void OrderBook::cancelAsk(std::string Username, double Price, int Quantity) {
    
}

std::string OrderBook::getBalance(std::string username) {
    if (users.find(username) != users.end()) {
        cout << "User found" << endl;
        cout << "User balance is as follows: " << endl;
        for (auto it = users[username].userBalance.balance.begin(); it != users[username].userBalance.balance.end(); ++it) {
            cout << it->first << ": " << fixed << setprecision(2) << it->second << endl;
        }
        return "Balance retrieved successfully.";
    } else {
        return "User " + username + " does not exist.";
    }
}

std::string OrderBook::getQuote(int qty) {
    
}

std::string OrderBook::getDepth() {
    
}

std::string OrderBook::addBalance(std::string Username, std::string market, int value) {
    
}

int main() {
    cout << "\n=========== " <<"WELCOME TO THE " << TICKER << " MARKET " << " =========== \n\n" << endl;
    return 0;
}