cmake_minimum_required(VERSION 3.16)

project(CookieClickerQt6 VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_add_executable(cookie_clicker
    src/main.cpp
)

target_link_libraries(cookie_clicker PRIVATE Qt6::Widgets)

install(TARGETS cookie_clicker
    BUNDLE DESTINATION .
    RUNTIME DESTINATION bin
)
