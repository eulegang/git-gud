# git_gud

A C project built with CMake.

## Build

```sh
cmake -S . -B build
cmake --build build
```

## Run

```sh
./build/git-gud
```

To run a supported hook directly (without installing it), pass the arguments Git would pass to the hook:

```sh
./build/git-gud run commit-msg path/to/commit-message
./build/git-gud run post-checkout <previous-ref> <new-ref> <0-or-1>
```

## Lint

Install `clang-tidy`, then configure and run the optional lint target:

```sh
cmake -S . -B build
cmake --build build --target lint
```

## Test

C unit tests use [Munit](https://nemequ.github.io/munit/) (vendored under `third_party/munit`, MIT license). [Bats](https://bats-core.readthedocs.io/) integration tests are registered when `bats` is available on `PATH`. Both run through CTest.

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

Use `-DBUILD_TESTING=OFF` when configuring to skip the test targets.
