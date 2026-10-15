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

## Lint

Install `clang-tidy`, then configure and run the optional lint target:

```sh
cmake -S . -B build
cmake --build build --target lint
```

## Test

Tests are written with [Bats](https://bats-core.readthedocs.io/) and registered with CTest when `bats` is available on `PATH`.

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```
