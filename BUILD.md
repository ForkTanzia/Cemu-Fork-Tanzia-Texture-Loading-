# Building Cemu-Fork-Tanzia (Windows)

This is a fork of Cemu; the build process is the same as upstream Cemu, plus this fork's
extra source files are already wired into the build. If you can build Cemu, you can build this.

## Requirements
- **Visual Studio 2022 or 2026** with:
  - Workload: **Desktop development with C++**
  - Individual components: **C++ CMake tools for Windows**, a **Windows 10/11 SDK**
- **Git**

## Get the source
Clone into a path with **no spaces** (a space breaks pkg-config during configure):
git clone --recursive https://github.com/YOURNAME/Cemu-Fork-Tanzia

If you forgot `--recursive`:

cd Cemu-Fork-Tanzia
git submodule update --init --recursive



## Build (Visual Studio)
1. Open the cloned folder in Visual Studio (**Open a local folder**).
2. Let **CMake configuration** finish (first run is slow — it downloads/builds dependencies).
3. Set config to **release** (or RelWithDebInfo) and target **CemuBin**.
4. **Build → Build All.** Output: `bin\Cemu_release.exe`.

## Build (command line)
From a **Developer Command Prompt** for your VS version:

cmake -S . -B build -G "Visual Studio 18 2026" -A x64
cmake --build build --config release

(Use `"Visual Studio 17 2022"` if building with VS 2022. Use the SAME VS for every rebuild.)

---

## Troubleshooting build errors

These are common when multiple Visual Studio toolsets are installed. Apply only if you hit them.

### `pkg-config` / `PkgConfig::libusb` error, or a path that stops at a space
Your source path contains a space. Move the repo to a space-free path (e.g. `C:\CemuTanzia`).

### `C1047` / `C1900` — "object was created by a different version of the compiler"
Two MSVC toolsets are being mixed under link-time optimization. Fix by disabling IPO/LTCG:
- In the **root `CMakeLists.txt`**, set both to `OFF`:

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE OFF)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO OFF)

- Delete `out/` and `build/`, then reconfigure.

### Toolset mismatch persists, or `LNK2019 __std_find_*` unresolved symbols
vcpkg reused libraries built by a different toolset. Force a clean dependency rebuild — from a
Developer Command Prompt:

set VCPKG_BINARY_SOURCES=clear
rmdir /s /q "%LOCALAPPDATA%\vcpkg\archives"
cd /d <repo>
rmdir /s /q dependencies\vcpkg\installed dependencies\vcpkg\buildtrees out build
cmake -S . -B build -G "Visual Studio 18 2026" -A x64
cmake --build build --config release



### Optional: disable libusb
libusb only provides USB-GameCube-controller support. If it causes trouble you can disable it:
- Root `CMakeLists.txt`: `option(ENABLE_LIBUSB "Build with libusb support" OFF)`
- `vcpkg.json`: remove the `"libusb"` line (and the now-trailing comma on the line above).

---

## Notes for this fork
- The texture-replacement source lives in `src/Cafe/HW/Latte/Core/LatteTextureReplace.{h,cpp}`
  and is already added to `src/Cafe/CMakeLists.txt`.
- `stb_image.h` is vendored in the same folder (used for PNG/TGA replacements).
- Custom-texture loading is enabled by default; put textures in `<Cemu>/load/textures/`.
