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
