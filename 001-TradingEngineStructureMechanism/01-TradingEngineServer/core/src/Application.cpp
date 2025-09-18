#include "../include/TradingEngineServer/core/Application.h"
#include "../include/TradingEngineServer/core/TradingEngineServer.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <csignal>
#include <atomic>

namespace TradingEngineServer::Core {
    static std::atomic<bool> g_terminate{false};
    static TradingEngineServer* g_server = nullptr;

    void signal_handler(int sig) {
        std::cout << "Signal " << sig << " received. Stopping server...\n";
        g_terminate = true;
        if (g_server) g_server->Stop();
    }

    int Application::Run(int argc, char* argv[]) {
        std::cout << "Trading Engine Application is starting..." << std::endl;

        // Setup signal handlers
        signal(SIGINT, signal_handler);
        signal(SIGTERM, signal_handler);

        // Start server
        TradingEngineServer server;
        g_server = &server;
        server.Start();

        std::cout << "Trading Engine Server is running. Press Ctrl+C to stop.\n";

        // Main loop waits for termination
        while (!g_terminate.load()) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        // Graceful shutdown
        server.Stop();
        std::cout << "Trading Engine Application has stopped.\n";
        return 0;
    }
}