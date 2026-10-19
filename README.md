# WebUI C++ Samples

Small C++ examples that use [WebUI](https://github.com/webui-dev/webui) to create desktop-style web interfaces. The samples demonstrate embedded HTML, C++/JavaScript events, charts, local web assets, and native WebView2 windows.

## Samples

| Target | Source | Description |
| --- | --- | --- |
| `minimal_sample` | `samples/minimal` | A minimal WebUI window with interactive buttons. |
| `chart_sample` | `samples/chart` | Draws a canvas chart using data generated in C++. |
| `richchart_sample` | `samples/richchart` | Displays charts using bundled Chart.js assets. |
| `plotly_sample` | `samples/plotly` | Demonstrates interactive Plotly charts with bundled assets. |
| `widgets_sample` | `samples/widgets` | Demonstrates interactive WebUI widgets. |
| `webview2_sample` | `samples/webview2` | Opens a native WebView2 window, with a browser fallback. |

The chart libraries and other sample assets are kept under each sample's `assets` directory and copied beside the executables during the build.

## Requirements

- Windows 10 or later
- CMake 3.20 or later
- Ninja
- A C++20 compiler: MSVC or MinGW-w64
- Microsoft Edge WebView2 Runtime for native WebView2 windows; a supported browser is needed for browser-based samples

The included CMake presets are for MSVC. They use the `x64-windows-static` vcpkg triplet and the project's `ports` overlay to provide WebUI. MinGW builds use a separately built or installed MinGW-compatible WebUI package.

## Configure and build with MSVC

Install [vcpkg](https://github.com/microsoft/vcpkg) and set `VCPKG_ROOT` to its installation directory. From the repository root, configure and build the Release preset:

```powershell
cmake --preset vcpkg_release
cmake --build build-msvc\vcpkg_release
```

To build Debug instead:

```powershell
cmake --preset vcpkg_debug
cmake --build build-msvc\vcpkg_debug
```

The executables are created in the selected build directory. For example, run a Release sample with:

```powershell
.\build-msvc\vcpkg_release\minimal_sample.exe
```

## Configure and build with MinGW

Install MinGW-w64 and build or install WebUI with the same MinGW toolchain. Make sure CMake can locate the WebUI package configuration file (`webuiConfig.cmake`). Then configure and build from the repository root, replacing the example path with the directory that contains that file:

```powershell
cmake -S . -B build-mingw -G Ninja `
  -DCMAKE_CXX_COMPILER=g++ `
  -DCMAKE_BUILD_TYPE=Release `
  -Dwebui_DIR="C:\path\to\webui\cmake\package"
cmake --build build-mingw
```

Run a sample with:

```powershell
.\build-mingw\minimal_sample.exe
```

On Windows, executables built with either compiler use the GUI subsystem and do not open a command prompt window.

## Install

The Release preset installs to `dist\vcpkg_release` by default:

```powershell
cmake --install build-msvc\vcpkg_release
```

The install directory contains the sample executables, their web assets, and the WebView2 loader and license files.
