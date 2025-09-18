#pragma once

namespace TradingEngineServer::Core {
    class ITradingEngineServer {
    public:
        virtual ~ITradingEngineServer() = default;

        virtual void Start() = 0;
        virtual void Stop() = 0;
        virtual void RunOnce() = 0;
    };
} 