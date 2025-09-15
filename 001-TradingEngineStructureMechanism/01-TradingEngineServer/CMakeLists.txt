cmake_minimum_required(VERSION 3.31.0)
project(TradingEngineServer 
    LANGUAGES CXX
    DESCRIPTION "Trading Engine Server Project"
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/core/include)

add_executable(TradingEngineServerExec
    core/src/Application.cpp
    server/src/main.cpp
)