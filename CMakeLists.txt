cmake_minimum_required(VERSION 3.10)
project(CursorTesterNative)

set(CMAKE_CXX_STANDARD 17)

add_executable(CursorTester WIN32 main.cpp)

if(WIN32)
    target_link_libraries(CursorTester user32 gdi32)
endif()