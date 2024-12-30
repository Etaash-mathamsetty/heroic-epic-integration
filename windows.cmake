set(CMAKE_SYSTEM_NAME Windows)

set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
set(WIN_LIBRARIES mingw32 libgcc.a libgcc_eh.a)

set(LINK_OPTIONS -municode)
