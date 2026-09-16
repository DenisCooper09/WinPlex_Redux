cmake_minimum_required(VERSION 4.3)
project(WinPlex_Redux)

set(CMAKE_CXX_STANDARD 20)

add_executable(WinPlex_Redux WIN32 src/main.cpp)

target_link_libraries(WinPlex_Redux PRIVATE d3d11 dxgi user32 gdi32)
