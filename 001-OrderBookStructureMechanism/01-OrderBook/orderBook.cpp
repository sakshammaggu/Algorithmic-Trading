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