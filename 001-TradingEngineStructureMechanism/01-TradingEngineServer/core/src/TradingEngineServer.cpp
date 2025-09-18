#include "../include/TradingEngineServer/core/TradingEngineServer.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace TradingEngineServer::Core {
    TradingEngineServer::TradingEngineServer() = default;

    TradingEngineServer::~TradingEngineServer() {
        Stop();
    }

    void TradingEngineServer::Start() {
        if (m_worker.joinable()) return;
        m_stopRequested = false;
        m_worker = std::thread(&TradingEngineServer::runLoop, this);
    }

    void TradingEngineServer::Stop() {
        m_stopRequested = true;
        m_cv.notify_all();
        if (m_worker.joinable()) m_worker.join();
    }

    void TradingEngineServer::RunOnce() {
        // Replace with real work
        std::cout << "[TradingEngineServer] RunOnce: doing work\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    void TradingEngineServer::runLoop() {
        std::cout << "[TradingEngineServer] Background thread started\n";
        while (!m_stopRequested.load(std::memory_order_acquire)) {
            RunOnce();
            std::unique_lock<std::mutex> lock(m_mtx);
            m_cv.wait_for(lock, std::chrono::milliseconds(500), [this] {
                return m_stopRequested.load(std::memory_order_acquire);
            });
        }
        std::cout << "[TradingEngineServer] Background thread exiting\n";
    }
}