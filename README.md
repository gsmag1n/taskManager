# taskManager

Qt Widgets application for the lab work "Менеджер задач на QListWidget".

## Features

- Add a task through `QInputDialog`.
- Reject empty tasks and duplicate task names.
- Remove the selected task with confirmation.
- Clear the whole list with confirmation.
- Edit a task by double-clicking an item.
- Show the number of tasks in the window title.
- Keep `QListWidget` in single-selection mode.

## Project structure

```text
CMakeLists.txt
src/
  main.cpp
  mainwindow.cpp
  mainwindow.h
  mainwindow.ui
```

## Build

Install Qt 5 or Qt 6 development packages, then run:

```bash
cmake -S . -B build
cmake --build build
./build/taskManager
```
