#include "orderBook.hpp"
#include <iostream>
#include <algorithm>
#include <iomanip>

using namespace std;

int Order::orderCounterBid = 0; // Initialize the static counter for BID orders
int Order::orderCounterAsk = 0; // Initialize the static counter for ASK orders

void OrderBook::flipBalance(const std::string& userId1, const std::string& userId2, double quantity, double price) {

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