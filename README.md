# Heroic Epic Integration

## How to Build
First install mingw on your distro, then
```
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../windows.cmake ..
make
mv heroic-epic-integration.exe EpicGamesLauncher.exe
```
Then copy the exe and use some bat script to run your game.
### Example:
```
start "" EpicGamesLauncher.exe PlayGTAV.exe %*
```
