# Preface
- You're trying to compile a C program, after all.

# Building the thing
> [!NOTE]
> If using Nix-shell, skip this part.

## Hard requirements
- A C compiler that has proper support for C99.
  - Newer is better though. There might be some smaller patches of later
    versions we rely on.
  - Probably will *not* work on MSVC, not because of the socket calls, but
    because of variable-length arrays. VLAs are heavily used in this project to
    pass arrays as parameters.

- CMake. Version >= 3.23.
  - If you use conan, you can use a lower CMake version, and trigger conan (see
    guide below) to obtain a new CMake version.
- libevent.

## Situational requirements
- Ninja, replacing Makefiles.

### Finding the dependencies
- Pick one of the below options if necessary:
  - conan package manager.
  - pkg-config.
  - Or supply your own `CMAKE_PREFIX_PATH`.

## Actually building and running the thing
### Nix shell
```bash
# at the project root
nix-shell
# you're in nix-shell now
cmake -DCMAKE_BUILD_TYPE=Debug -B build
# if you use Ninja
# -G Ninja
# if you use a language server
# -DCMAKE_EXPORT_COMPILE_COMMAND=ON

cmake --build build
./build/server
# on a different terminal, or tmux window
./build/client
```

### Conan
```bash
# if you have never done so before, run the line below
conan profile detect
# if you use ninja, edit ~/.conan2/profiles/default
# and add:
# 
# [conf]
# tools.cmake.cmaketoolchain:generator=Ninja

# then,
conan install . --output-folder=build -s build_type=Debug -b missing

# then wait, and then
source ./build/conanrun.sh
# now your CMake should be of version 4.x.x
cmake --version

cmake --preset conan-debug
cmake --build build
./build/server
# on a different terminal, or tmux window
./build/client
```

### No pkg-config
- If library is installed in a non-standard path, make sure to supply
  `CMAKE_PREFIX_PATH`.
```bash
cmake -B build
# -G Ninja\
# -DCMAKE_BUILD_TYPE=Debug\
# -DCMAKE_PREFIX_PATH="non-standard/path/to/event2"
cmake --build build
```

### With pkg-config
- If the pc file is installed in a non-standard path, make sure to supply
  `CMAKE_PREFIX_PATH`.
```bash
cmake -B build
# -G Ninja\
# -DCMAKE_BUILD_TYPE=Debug\
# -DCMAKE_PREFIX_PATH="non-standard/path/to/pc/file/of/libevent"
cmake --build build
```

## Troubleshooting
### Trying to use locally installed package after having configured to use conan
- First, simply try to remove `build/CMakeCache.txt`, and rerun CMake as listed
  in 
  [the build guide using system package manager and pkg-config.](#if-all-dependencies-have-been-installed-via-system-package-manager)
- If the build fails because CMake still finds the package built by conan
  (whose path is prefixed with `$HOME/.conan2/`), remove
  `~/.cmake/packages/<package-name>` (Unix) or
  `HKEY_CURRENT_USER\Software\Kitware\CMake\Packages\<package-name>` (Windows),
  substituting `<package-name>` with that of the package that's not supposed to be
  found.
  - To be more precise, in that path, there are some files whose contents is
    the path to the library/package/$\dots$. Removing only the file that points
    to the path with prefix `~/.conan2` is sufficient.
  - The path just removed is CMake's [user package registry](https://cmake.org/cmake/help/v3.31/manual/cmake-packages.7.html#user-package-registry)
    feature.
