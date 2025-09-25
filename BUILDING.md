# Preface
- You're trying to compile a C program, after all.

# Building the thing
## Requirements
- If you have Nix Shell, you can ignore all of the below.
- CMake (preferably >= 3.23 for use of CMakePresets.json).
- [conan](https://conan.io/), then you can skip other requirements.
  - If you use conan, you can ignore the requirements below.
- [pkgconf](http://pkgconf.org/) or its more popular brother `pkg-config`.
- A build system (preferably ninja for speed, but make is fine and everyone has
  it).
- libevent. Since `select` and `poll` are Unix only and runs in linear time.
  The non-portable `epoll` (Linux) or `kqueue` (BSDs) are more efficient but
  only work on one platform.
  - If you manually install libevent, make sure `libevent.pc` is discoverable
    by `pkg-config` and/or `pkgconf`.
  - If you use neither of those, when running `cmake -B build`, add
    `-DCMAKE_C_FLAGS='-Wl,-rpath=/path/to/libevent.so;-I/path/to/dir/event2'`
    (on Mac, it's `.dylib` instead of `.so`). On Windows, I'm not sure.

- AddressSanitizer and UndefinedBehaviorSanitizer are nice to catch bugs.
  - Unless you use a very old version of gcc or clang, both of these should
    already be possible compile features in the compiler.

- What's the TL;DR:
  - Just use Nix Shell if you want to save the headache.
  - If you want a bit more headache, use conan.

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
# on a different terminal, or tmux session
./build/client
```

### Conan
```bash
# if you have never done so, run the line below
conan profile detect
# then,
conan install . --output-folder=build -s build_type=Debug -b missing
# then wait, then
source ./build/conanrun.sh
# now your CMake should be of version 4.x.x
cmake --version

cmake --preset conan-debug
cmake --build build
./build/server
# on a different terminal, or tmux session
./build/client
```

### If all dependencies have been installed via system package manager
```bash
# these all should work
cmake --version
# either pkg-config or pkgconf
# pkg-config --version
# pkgconf --version
# let's say pkg-config
pkg-config --libs libevent
# if you use ninja
# ninja --version
cmake -B build -DCMAKE_BUILD_TYPE=Debug
# if you use ninja
# -G Ninja
# if you use a language server
# -DCMAKE_EXPORT_COMPILE_COMMAND=ON
cmake --build build
./build/server
# on a different terminal, or tmux session
./build/client
```
