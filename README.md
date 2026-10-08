# git_gud

A C project built with CMake.

## Build

```sh
cmake -S . -B build
cmake --build build
```

## Run

```sh
./build/git_gud
```

## Test

Tests are written with [Bats](https://bats-core.readthedocs.io/) and registered with CTest when `bats` is available on `PATH`.

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```
