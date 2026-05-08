cmake_minimum_required(VERSION 3.16)

project(CookieClickerQt6 VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_add_executable(cookie_clicker
    src/AnimationEffects.cpp
    src/AnimationEffects.h
    src/CookieButton.cpp
    src/CookieButton.h
    src/CookieClickerWindow.cpp
    src/CookieClickerWindow.h
    src/CookieGame.cpp
    src/CookieGame.h
    src/main.cpp
    src/NumberFormatter.cpp
    src/NumberFormatter.h
    src/Upgrade.h
    src/UpgradeCard.cpp
    src/UpgradeCard.h
    src/UpgradeCatalog.cpp
    src/UpgradeCatalog.h
)

target_link_libraries(cookie_clicker PRIVATE Qt6::Widgets)

install(TARGETS cookie_clicker
    BUNDLE DESTINATION .
    RUNTIME DESTINATION bin
)
