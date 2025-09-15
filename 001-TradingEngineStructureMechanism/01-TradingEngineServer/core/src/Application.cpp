#include "../include/TradingEngineServer/core/Application.h"
#include <iostream>

namespace TradingEngineServer::Core {
    int Application::Run(int argc, char* argv[]) {
        std::cout << "Trading Engine Application is running..." << std::endl;
        return 0;
    }
}