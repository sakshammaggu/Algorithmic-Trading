#include "orderBook.hpp"
#include <iostream>
#include <algorithm>
#include <iomanip>

using namespace std;

int Order::orderCounterBid = 0; // Initialize the static counter for BID orders
int Order::orderCounterAsk = 0; // Initialize the static counter for ASK orders

/**
 * @brief Executes a balance transfer between two users during a trade
 * 
 * @details This function handles the actual transaction between buyer and seller:
 *          1. Transfers USD from buyer to seller
 *          2. Transfers stocks from seller to buyer
 *          3. Validates balances before transfer
 * 
 * @param userId1 The buyer's username (receiving stocks, paying USD)
 * @param userId2 The seller's username (receiving USD, giving stocks)
 * @param quantity Number of stocks to transfer
 * @param price Price per stock for the transaction
 * 
 * @note Transaction will only proceed if:
 *       - Both users exist
 *       - Buyer has sufficient USD balance
 *       - Seller has sufficient stock balance
 * 
 * @return void, but prints transaction status to console
 */
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
/**
 * @brief Constructor for the OrderBook class
 * 
 * @details Initializes the order book with three market makers and their initial positions:
 *          1. Creates three market maker users (MM1, MM2, MM3)
 *          2. Sets initial balances for each market maker
 *          3. Places initial orders to create market liquidity
 * 
 * Initial setup:
 * - MarketMaker1: 10000 USD, 1000 GOOGL
 * - MarketMaker2: 10000 USD, 2000 GOOGL
 * - MarketMaker3: 50000 USD, 0 GOOGL
 * 
 * @note This constructor establishes the initial market state with
 *       a mix of bid and ask orders to ensure market liquidity
 */
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

/**
 * @brief Creates a new user in the trading system
 * 
 * @details Creates a new user with:
 *          - Empty initial balances
 *          - Unique username validation
 * 
 * @param username The desired username for the new user
 * 
 * @return string Status message indicating:
 *         - Success: "User {username} created successfully"
 *         - Failure: "User {username} already exists"
 * 
 * @note New users start with zero balance in all currencies/stocks
 */
string OrderBook::makeUser(std::string username) {
    User newUser(username);
    if (users.find(username) == users.end()) {
        users[username] = newUser;
        return "User " + username + " created successfully.";
    } 
    return "User " + username + " already exists.";
}

/**
 * @brief Places a new bid (buy) order in the order book
 * 
 * @details Algorithm:
 * 1. Validates user existence and USD balance
 * 2. Sorts existing ask orders by price (ascending) and time priority
 * 3. Matches against existing ask orders if possible:
 *    - Fully matches and removes completed ask orders
 *    - Partially matches and updates remaining quantities
 * 4. Places remaining quantity as new bid if not fully matched
 * 
 * @param Username The username of the bidder
 * @param Price The maximum price willing to pay per stock
 * @param Quantity The number of stocks to buy
 * 
 * @return string Status message indicating:
 *         - Error if user doesn't exist
 *         - Error if insufficient USD balance
 *         - Success message with trade details
 * 
 * @note 
 * - Implements price-time priority matching
 * - Matches at ask price, not bid price
 * - Requires sufficient USD balance: Price * Quantity
 */
string OrderBook::addBid(std::string Username, double Price, int Quantity) {
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

/**
 * @brief Places a new ask (sell) order in the order book
 * 
 * @details Algorithm:
 * 1. Validates user existence and stock balance
 * 2. Sorts existing bid orders by price (descending) and time priority
 * 3. Matches against existing bid orders if possible:
 *    - Fully matches and removes completed bid orders
 *    - Partially matches and updates remaining quantities
 * 4. Places remaining quantity as new ask if not fully matched
 * 
 * @param Username The username of the seller
 * @param Price The minimum price willing to accept per stock
 * @param Quantity The number of stocks to sell
 * 
 * @return string Status message indicating:
 *         - Error if user doesn't exist
 *         - Error if insufficient stock balance
 *         - Success message with trade details
 * 
 * @note 
 * - Implements price-time priority matching
 * - Matches at bid price, not ask price
 * - Requires sufficient stock balance: Quantity
 */
string OrderBook::addAsk(std::string Username, double Price, int Quantity) {
    // First check if the username exists in the users map
    if (users.find(Username) == users.end()) {
        return "Error: User " + Username + " does not exist.";
    }

    // Check if user has enough GOOGL balance for the ask
    if (users[Username].userBalance.balance[TICKER] < Quantity) {
        return "Error: User " + Username + " does not have enough " + TICKER + " balance for this ask.";
    }

    int remQty = Quantity;

    stable_sort(bids.begin(), bids.end(), [](const Order &a, const Order &b) {
        // If prices are equal, maintain the original order
        if (a.price == b.price) {
            return a.insertionOrderBid < b.insertionOrderBid; // Earlier insertion order has higher priority
        }
        return a.price > b.price; // sort by price
    });

    for (auto it = bids.begin(); it != bids.end();) {
        if (remQty > 0  && Price <= it->price) {
            if (it->quantity > remQty) {
                it->quantity -= remQty;
                flipBalance(it->userName, Username, it->quantity, it->price);
                cout << "Ask Satisfied Successfully at price: " << it->price << " and quantity: " << remQty << endl;
                remQty = 0;
                break;
            } else {
                remQty -= it->quantity;
                flipBalance(it->userName, Username, it->quantity, it->price);
                cout << "Ask Satisfied Partially at price: " << it->price << " and quantity: " << it->quantity << endl;
                it = bids.erase(it); // get the next valid iterator after erasing
            }
        } else {
            ++it;
        }
    }

    if (remQty > 0) {
        Order ask(Username, "ask", Price, remQty);
        asks.push_back(ask);
        cout << "Remaining quantity of asks added to Orderbook" << endl;
    }

    if (remQty == 0) {
        cout << "Complete Ask Satisfied Successfully" << endl;
    }

    return "Ask added successfully."; 
}

/**
 * @brief Cancels an existing bid order
 * 
 * @details Search and cancellation logic:
 * 1. Searches for exact match (username, price, quantity)
 * 2. If found with larger quantity, reduces the quantity
 * 3. If found with smaller quantity, rejects cancellation
 * 4. Removes order completely if quantities match
 * 
 * @param Username The username who placed the bid
 * @param Price The price of the bid to cancel
 * @param Quantity The quantity to cancel
 * 
 * @note 
 * - Cannot cancel more than existing quantity
 * - Requires exact match of all parameters
 * - Prints status messages to console
 */
void OrderBook::cancelBid(std::string Username, double Price, int Quantity) {
    for (auto it = bids.begin(); it != bids.end(); it++){
        if (it->userName == Username && it->price == Price && it->quantity == Quantity) {
            bids.erase(it);
            cout << "Bid cancelled successfully!" << endl;
            return;
        } else if (it->userName == Username && it->price == Price && it->quantity > Quantity) {
            it->quantity -= Quantity;
            cout << "Bid cancelled successfully!" << endl;
            return;
        } else if (it->userName == Username && it->price == Price && it->quantity < Quantity) {
            cout << "Cannot cancel more than existing quantity. Existing quantity is " << it->quantity << endl;
            cout << "Bid quantity is less than the quantity you want to cancel." << endl;
            cout << "Please enter the right quantity to cancel and retry!" << endl;
            return;
        }
    }

    cout << "Bid not found!! Please enter the right Username, Price and Quantity to cancel the bid!" << endl;
    return;
}

/**
 * @brief Cancels an existing ask order
 * 
 * @details Search and cancellation logic:
 * 1. Searches for exact match (username, price, quantity)
 * 2. If found with larger quantity, reduces the quantity
 * 3. If found with smaller quantity, rejects cancellation
 * 4. Removes order completely if quantities match
 * 
 * @param Username The username who placed the ask
 * @param Price The price of the ask to cancel
 * @param Quantity The quantity to cancel
 * 
 * @note 
 * - Cannot cancel more than existing quantity
 * - Requires exact match of all parameters
 * - Prints status messages to console
 */
void OrderBook::cancelAsk(std::string Username, double Price, int Quantity) {
    for (auto it = asks.begin(); it != asks.end(); it++){
        if (it->userName == Username && it->price == Price && it->quantity == Quantity) {
            asks.erase(it);
            cout << "Ask cancelled successfully!" << endl;
            return;
        } else if (it->userName == Username && it->price == Price && it->quantity > Quantity) {
            it->quantity -= Quantity;
            cout << "Ask cancelled successfully!" << endl;
            return;
        } else if (it->userName == Username && it->price == Price && it->quantity < Quantity) {
            cout << "Cannot cancel more than existing quantity. Existing quantity is " << it->quantity << endl;
            cout << "Ask quantity is less than the quantity you want to cancel." << endl;
            cout << "Please enter the right quantity to cancel and retry!" << endl;
            return;
        }
    }
}

/**
 * @brief Retrieves current market quote for buying specified quantity
 * 
 * @details Quote generation process:
 * 1. Sorts ask orders by price (ascending) and time priority
 * 2. Accumulates available quantities at each price level
 * 3. Shows all price levels needed to fulfill requested quantity
 * 
 * @param qty The quantity of stocks to get quote for
 * 
 * @return string "Quote retrieved successfully"
 * 
 * @note 
 * - Shows best available prices first
 * - May show multiple price levels if single price can't fulfill quantity
 * - Prints quote details to console
 * - Does not actually execute any trades
 */
string OrderBook::getQuote(int qty) {
    // Implementation of getQuote
    // We will need to find lowest ask prices till the qty passed in is met we keep displaying lowest ask prices

    stable_sort(asks.begin(), asks.end(), [](const Order &a, const Order &b) {
        // If prices are equal, maintain the original order
        if (a.price == b.price) {
            return a.insertionOrderAsk < b.insertionOrderAsk; // Compare based on the order of insertion
        }
        return a.price < b.price;   // Otherwise, sort by price
    });

    for (auto it = asks.begin(); it != asks.end(); ++it){
        if (qty > 0 && qty <= it->quantity) {
            cout << TICKER << "-> "
                 << "Quantity available: " << qty << " at " << it->price << " USD" << endl; // make the output look better
            return "Quote retrieved successfully.";
        } else if (qty > 0 && qty > it->quantity) {
            cout << TICKER << "-> "
                 << "Quantity available: " << it->quantity << " at " << it->price << " USD" << endl;
            qty -= it->quantity;
        } else {
            return "Quote retrieved successfully.";
        }
    }

    cout << "Quote retrieved successfully." << endl;
    return "Quote retrieved successfully.";
}

/**
 * @brief Displays the full order book depth (all bids and asks)
 * 
 * @details Display format:
 * 1. Sorts asks by price (descending)
 * 2. Sorts bids by price (descending)
 * 3. Shows asks above market (in red)
 * 4. Shows bids below market (in green)
 * 5. Each line shows price and quantity
 * 
 * @return string Formatted order book depth string
 * 
 * @note 
 * - Uses ANSI color codes for better visibility
 * - Red for asks (selling)
 * - Green for bids (buying)
 * - Shows full depth of market
 * - Prints to console and returns as string
 */
string OrderBook::getDepth() {
    // Sort asks in descending order of price
    sort(asks.begin(), asks.end(), [](const Order &a, const Order &b) { 
        return a.price > b.price; 
    });

    // Sort bids in descending order of price
    sort(bids.begin(), bids.end(), [](const Order &a, const Order &b) { 
        return a.price > b.price; 
    });

    string depthString = TICKER + " Depth:\n";

    for (const auto &ask : asks) {
        depthString += "\x1b[31m"; // Set color to red
        depthString += "Price: " + std::to_string(ask.price) + ", Quantity: " + std::to_string(ask.quantity) + "\n";
        depthString += "\x1b[0m"; // Reset color to default
    }

    depthString += "Asks above:\n";
    depthString += "Bids below:\n";

    for (const auto &bid : bids) {
        depthString += "\x1b[32m"; // Set color to green
        depthString += "Price: " + std::to_string(bid.price) + ", Quantity: " + std::to_string(bid.quantity) + "\n";
        depthString += "\x1b[0m"; // Reset color to default
    }

    cout << depthString << endl;
    return depthString;
}

/**
 * @brief Retrieves and displays all balances for a user
 * 
 * @details Balance display:
 * 1. Checks if user exists
 * 2. Shows all currency/stock balances
 * 3. Formats numbers with 2 decimal places
 * 
 * @param username The username whose balance to check
 * 
 * @return string Status message:
 *         - "Balance retrieved successfully" if user exists
 *         - "User {username} does not exist" if not found
 * 
 * @note 
 * - Shows all assets (USD, GOOGL, etc.)
 * - Uses fixed precision (2 decimals)
 * - Prints balances to console
 */
string OrderBook::getBalance(std::string username) {
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

/**
 * @brief Adds balance to a user's account for a specific market
 * 
 * @details Balance addition process:
 * 1. Validates user existence
 * 2. Adds specified value to the market balance
 * 3. Creates new balance entry if market doesn't exist
 * 
 * @param Username The username to add balance to
 * @param market The market/currency to add (e.g., "USD", "GOOGL")
 * @param value The amount to add to the balance
 * 
 * @return string Status message:
 *         - "Balance added successfully" if successful
 *         - "User not found" if user doesn't exist
 * 
 * @note 
 * - Can add both USD and stock balances
 * - Creates new market balance if not existing
 * - Prints status message to console
 */
string OrderBook::addBalance(std::string Username, std::string market, int value) {
    if (users.find(Username) != users.end()) {
        users[Username].userBalance.addBalances(market, value);
        cout << "Balance added successfully" << endl;
        return "Balance added successfully";
    }

    cout << "User not found!! Please enter the right Username to add balance!" << endl;
    return "User not found";
}

/**
 * @brief Main function implementing the trading platform interface
 * 
 * @details Menu options:
 * 1. Sign Up User - Create new trading account
 * 2. Add Balance - Add funds/stocks to user account
 * 3. Check Market Prices - View order book depth
 * 4. Add Bid - Place buy order
 * 5. Add Ask - Place sell order
 * 6. Get Quote - Check price for quantity
 * 7. Check Balance - View user balances
 * 8. Cancel Bid - Cancel buy order
 * 9. Cancel Ask - Cancel sell order
 * 10. Exit - Close platform
 * 
 * @return int Exit status (0 for normal exit)
 * 
 * @note 
 * - Creates initial market makers on startup
 * - Runs in continuous loop until exit
 * - Handles all user input validation
 */
int main() {
    OrderBook EXCH;

    string market;
    int choice;
    string username;
    int price, quantity;

    cout << "\n=========== " <<"WELCOME TO THE " << TICKER << " MARKET " << " =========== \n\n" << endl;
    cout << "\n=========== " << "CURRENT MARKET PRICES " << " =========== " << endl;
    EXCH.getDepth(); // getDepth() is called to display the current market

    while (true) {
        cout << "\n=========== " << TICKER << " Trading Platform ===========\n\n";
        cout << "\n========== Trading Platform Menu ==========\n";
        cout << "1. Sign Up User\n";
        cout << "2. Add Balance to User Account\n";
        cout << "3. Check Current Market Prices\n";
        cout << "4. Add Bid to " << TICKER << " v USD market\n";
        cout << "5. Sell your stocks in " << TICKER << " v USD Market\n";
        cout << "6. Get Current Quote to buy " << TICKER << " stocks\n";
        cout << "7. Check your current User Balance\n";
        cout << "8. Cancel Bid\n";
        cout << "9. Cancel Ask\n";
        cout << "10. Exit\n\n";
        cout << "Enter your choice: ";

        cin >> choice;

        switch (choice) {
            case 1:
                cout << "Enter username for new user: \n";
                cin >> username;
                EXCH.makeUser(username);
                break;
            case 2:
                cout << "Enter username to add balance: \n";
                cin >> username;
                cout << "Enter market (e.g., USD): \n";
                cin >> market;
                cout << "Enter balance value: \n";
                int value;
                cin >> value;
                EXCH.addBalance(username, market, value);
                break;
            case 3:
                EXCH.getDepth();
                break;
            case 4:
                cout << "Enter username for bid: \n";
                cin >> username;
                cout << "Enter bid price: \n";
                cin >> price;
                cout << "Enter bid quantity: \n";
                cin >> quantity;
                EXCH.addBid(username, price, quantity);
                break;
            case 5:
                cout << "Enter username for ask: \n";
                cin >> username;
                cout << "Enter ask price: \n";
                cin >> price;
                cout << "Enter ask quantity: \n";
                cin >> quantity;
                EXCH.addAsk(username, price, quantity);
                break;
            case 6:
                cout << "Enter quantity for quote: \n";
                cin >> quantity;
                EXCH.getQuote(quantity);
                break;
            case 7:
                cout << "Enter username to get balance: \n";
                cin >> username;
                EXCH.getBalance(username);
                break;
            case 8:
                cout << "Enter username to cancel bid: \n";
                cin >> username;
                cout << "Enter bid price: \n";
                cin >> price;
                cout << "Enter bid quantity: \n";
                cin >> quantity;
                EXCH.cancelBid(username, price, quantity);
                break;
            case 9:
                cout << "Enter username to cancel ask: \n";
                cin >> username;
                cout << "Enter ask price: \n";
                cin >> price;
                cout << "Enter ask quantity: \n";
                cin >> quantity;
                EXCH.cancelAsk(username, price, quantity);
                break;
            case 10:
                cout << "Exiting the trading platform. Goodbye!\n\n";
                return 0;
            default:
                cout << "Invalid choice. Please try again.\n\n";
        }
    }

    return 0;
}