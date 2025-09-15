# Building and Running the Trading Engine Server

## Prerequisites
- CMake
- MinGW (GCC/G++)

## Build Instructions
Open PowerShell and run the following commands:

```powershell
cd "D:/Algorithmic Trading/Algorithmic Trading/001-TradingEngineStructureMechanism/01-TradingEngineServer"
Remove-Item -Path build -Recurse -Force -ErrorAction SilentlyContinue
New-Item -ItemType Directory -Path build
cd build
cmake .. -G "MinGW Makefiles"
cmake --build .
```

## Run the Executable
From the `build` directory, run:

```powershell
./TradingEngineServerExec
```
