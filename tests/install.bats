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

@test "install adds itself to the hooks" {
  run "$GIT_GUD_BINARY" install

  [ "$status" -eq 0 ]
	[ -f "$TEST_REPO/.git/hooks/git-gud" ]

  run "$GIT_GUD_BINARY" install
  [ "$status" -eq 1 ]
	[ -f "$TEST_REPO/.git/hooks/git-gud" ]

  run "$GIT_GUD_BINARY" install -f
  [ "$status" -eq 0 ]
	[ -f "$TEST_REPO/.git/hooks/git-gud" ]
}
