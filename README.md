# 2D Map Navigator

**Course:** Computer Graphics

## Project Description

This project is a 2D graphical application for measuring movement on a map, developed in C++ using OpenGL (version ≥3) with a programmable pipeline. The application allows the user to either navigate a map with a central pin or measure distances between points on the map, while preserving state when switching between modes.

---

## Features

### Walking Mode

- Active at the start of the program.
- Displays only a portion of a large map.
- A central pin remains fixed while the map moves relative to keyboard input (`W`, `A`, `S`, `D`).
- The distance traveled is continuously measured and displayed.
- A walking icon is shown on the screen to indicate the current mode.
- Pressing `R` or clicking the icon switches to Distance Measurement mode.

### Distance Measurement Mode

- Displays the full map.
- Left-clicking adds a white point; consecutive clicks create line segments between points.
- Total distance of all segments is calculated and displayed.
- Clicking on an existing point deletes it and reconnects its neighbors if needed.
- A ruler icon is displayed to indicate the current mode.
- Pressing `R` or clicking the icon switches back to Walking mode.

### Additional Features

- Cursor appears as a compass, with the red needle pointing to the top-left corner.
- Full-screen display.
- Pressing `Escape` closes the program.
- Supports shader programming, uniforms, transparent colors/textures, and mouse/keyboard input.
- Text rendering implemented with FreeType library.

---

## Controls

| Action                       | Key / Input               |
| ---------------------------- | ------------------------- |
| Move Up                      | W                         |
| Move Down                    | S                         |
| Move Left                    | A                         |
| Move Right                   | D                         |
| Switch Mode                  | R or click icon           |
| Add point (Distance Mode)    | Left Mouse Click          |
| Delete point (Distance Mode) | Left Mouse Click on point |
| Exit Program                 | Escape                    |

---

## Technical Requirements

- Developed in **C++** with **OpenGL ≥3**.
- Programmable pipeline used.
- Full screen rendering with frame limiting at 75 FPS.
- Proper handling of state between modes.

---

## Compilation & Execution

1. Clone the repository:
   git clone https://github.com/anasinik/map-navigator-2d
   cd map-navigator-2d

2. Build with CMake (Linux / Windows / Mac):

   ```
   mkdir build
   cd build
   cmake ..
   cmake --build .
   ```

   Run the executable:

   - On Windows: MapNavigator2D.exe
   - On Linux/Mac: ./MapNavigator2D

OR

2. Build with Visual Studio (Windows):
   Open the generated .sln file in Visual Studio.
   Build the solution in Release or Debug mode.
   Run MapNavigator2D.exe
