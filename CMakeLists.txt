cmake_minimum_required(VERSION 3.16)

project(CookieClicker
    VERSION 1.0.0
    DESCRIPTION "A cookie-themed Qt6 idle clicker with upgrades and animations."
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

find_package(Qt6 6.2 REQUIRED COMPONENTS Core Gui Widgets)

qt_standard_project_setup()

set(COOKIE_SOURCES
    src/main.cpp
    src/Game.cpp
    src/Game.h
    src/MainWindow.cpp
    src/MainWindow.h
    src/CookieButton.cpp
    src/CookieButton.h
    src/UpgradeCard.cpp
    src/UpgradeCard.h
    src/UpgradePanel.cpp
    src/UpgradePanel.h
    src/FloatingText.cpp
    src/FloatingText.h
    src/Toast.cpp
    src/Toast.h
    src/Format.cpp
    src/Format.h
)

qt_add_executable(cookie_clicker ${COOKIE_SOURCES})

target_include_directories(cookie_clicker PRIVATE src)

target_link_libraries(cookie_clicker PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

set_target_properties(cookie_clicker PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)

if(MSVC)
    target_compile_options(cookie_clicker PRIVATE /W4 /permissive-)
else()
    target_compile_options(cookie_clicker PRIVATE -Wall -Wextra -Wpedantic)
endif()

install(TARGETS cookie_clicker
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION bin
)
