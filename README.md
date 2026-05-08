# Cookie Clicker Qt6

A cookie-themed Qt6 desktop clicker game with animated clicking, floating score text,
passive baking, and purchasable upgrades.

## Features

- Big animated cookie button with glow, pulse, and press effects
- Cookie counter, per-click power, passive cookies-per-second, and all-time total
- Five upgrades: Rolling Pin, Chocolate Chips, Grandma's Oven, Sprinkle Storm, and Golden Mixer
- Floating feedback text and upgrade pulse animations
- Warm bakery-themed Qt Widgets styling

## Build

Install Qt6 development packages and CMake, then run:

```sh
CXX=g++ cmake -S . -B build
cmake --build build
```

## Run

```sh
./build/cookie_clicker
```
