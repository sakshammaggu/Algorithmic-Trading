#pragma once
#include "TradingEngineServerInterface.h"
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <chrono>

namespace TradingEngineServer::Core {
    class TradingEngineServer : public ITradingEngineServer {
    public:
        TradingEngineServer() noexcept;
        ~TradingEngineServer() override;

        TradingEngineServer(const TradingEngineServer&) = delete;
        TradingEngineServer& operator=(const TradingEngineServer&) = delete;

        void Start() override;
        void Stop() override;
        void RunOnce() override; 

    private:
        void runLoop(); 
        std::atomic<bool> m_stopRequested;
        std::thread m_worker;
        std::mutex m_mtx;
        std::condition_variable m_cv;
    };
}