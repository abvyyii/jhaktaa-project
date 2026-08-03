# Jhatkaa

Digital logic gate simulator built with Qt and CMake.

## Project layout

- `src/` — application source code
- `CMakeLists.txt` — root CMake project
- `src/CMakeLists.txt` — Qt executable target

## Requirements

- CMake 3.16 or newer
- C++17-capable compiler
- Qt Widgets development libraries (Qt 6 or Qt 5)

## Build on Windows

1. Open a terminal with Qt environment available.
   - If using Visual Studio, use `x64 Native Tools Command Prompt` and make sure Qt is installed for MSVC.
   - If using MSYS2, open the MSYS2 MinGW shell and install Qt and Ninja.
   Using the UCRT64 terminal is recommended

2. Configure and generate the build files:

```powershell
cmake -S . -B build
```

3. Build the project:

```powershell
cmake --build build --config Release
```

4. Run the executable:

```powershell
build\src\jhatkaa.exe
```
P.S. Ensure that you are using the UCRT64 terminal saved directly into your C drive

## If friends clone this repo

Tell them to:

```powershell
git clone https://github.com/<your-name>/<repo>.git
cd <repo>
cmake -S . -B build
cmake --build build --config Release
build\src\jhatkaa.exe
```

## Notes

- Do not commit the `build/` directory to GitHub.
- If you want to share a runnable binary, use GitHub Releases or attach the built `.exe` and required Qt redistributables.
