#!/usr/bin/env bats

setup() {
  test -n "${GIT_GUD_BINARY:-}"
  test -x "$GIT_GUD_BINARY"

  export TEST_REPO
  TEST_REPO="$(mktemp -d)"
  git -C "$TEST_REPO" init --quiet
  cd "$TEST_REPO"
}

teardown() {
  rm -rf "$TEST_REPO"
}

@test "version reports command version and missing hook" {
  run "$GIT_GUD_BINARY" version

  [ "$status" -eq 0 ]
  [[ "$output" == *"cmd: "* ]]
  [[ "$output" == *"hook: -"* ]]
}

@test "version checks the current repository by default" {
  run "$GIT_GUD_BINARY" install
  [ "$status" -eq 0 ]

  run "$GIT_GUD_BINARY" version
  [ "$status" -eq 0 ]
  [[ "$output" == *"cmd: "* ]]
  [[ "$output" == *"hook: "* ]]
  [[ "$output" != *"hook: -"* ]]
}

@test "version checks an installation in a directory outside the repository" {
  mkdir "$TEST_REPO/custom-hooks"
  cd "$(dirname "$TEST_REPO")"
  hooks_dir="$(basename "$TEST_REPO")/custom-hooks"

  run "$GIT_GUD_BINARY" version "$hooks_dir"
  [ "$status" -eq 0 ]
  [[ "$output" == *"hook: -"* ]]

  run "$GIT_GUD_BINARY" install "$hooks_dir"
  [ "$status" -eq 0 ]

  run "$GIT_GUD_BINARY" version "$hooks_dir"
  [ "$status" -eq 0 ]
  [[ "$output" == *"cmd: "* ]]
  [[ "$output" == *"hook: "* ]]
  [[ "$output" != *"hook: -"* ]]
}

@test "version rejects extra directories" {
  run "$GIT_GUD_BINARY" version . .
  [ "$status" -eq 1 ]
}
