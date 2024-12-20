set(CMAKE_SYSTEM_NAME Windows)

set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
set(WIN_LIBRARIES mingw32 opengl32 uuid gdi32 comdlg32 dwmapi libstdc++.a libgcc.a libgcc_eh.a libwinpthread.a)

add_link_options(-municode)
