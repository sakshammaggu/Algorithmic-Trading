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

std::string OrderBook::addBid(std::string Username, double Price, int Quantity) {

}

std::string OrderBook::addAsk(std::string Username, double Price, int Quantity) {
    
}

void OrderBook::cancelBid(std::string Username, double Price, int Quantity) {
    
}

void OrderBook::cancelAsk(std::string Username, double Price, int Quantity) {
    
}

std::string OrderBook::getBalance(std::string username) {
    
}

std::string OrderBook::getQuote(int qty) {
    
}

std::string OrderBook::getDepth() {
    
}

std::string OrderBook::makeUser(std::string username) {
    
}

std::string OrderBook::addBalance(std::string Username, std::string market, int value) {
    
}

int main() {
    cout << "\n=========== " <<"WELCOME TO THE " << TICKER << " MARKET " << " =========== \n\n" << endl;
    return 0;
}