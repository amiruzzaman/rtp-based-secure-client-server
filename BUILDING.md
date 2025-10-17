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
### Nix-shell and language servers (specifically clangd)
- Problem: even if the project successfully builds inside `nix-shell`, your
  `clangd` (or whatever language server you use) setup might not be able to
  detect some library headers (for example, `<event2/event.h>`).
- Fix:
  - Use `nix-shell dev-shell.nix` instead. Run the
    [nix-shell build steps](#nix-shell) and build. And also launch your editor
    in the same environment (that is, another session that's running
    `nix-shell dev-shel.nix`).
- Another possible fix:
  1. Enter `nix-shell`.
  2. `echo $NIXPKGS_CMAKE_PREFIX_PATH` and copy the output.
    - Or any way to be able to save and read the value of
      `$NIXPKGS_CMAKE_PREFIX_PATH` again after exiting nix-shell.
  3. Exit `nix-shell`.
  4. Supply `CMAKE_PREFIX_PATH` environment variable using the value copied.
    - This is different from the instructions above.

```bash
CMAKE_PREFIX_PATH="copied-value" cmake -B build
# now you can build outside of nix-shell also
cmake --build build
```
