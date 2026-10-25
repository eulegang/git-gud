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

@test "install accepts a hooks directory outside a repository" {
  mkdir "$TEST_REPO/custom-hooks"
  cd "$(dirname "$TEST_REPO")"
  hooks_dir="$(basename "$TEST_REPO")/custom-hooks"

  run "$GIT_GUD_BINARY" install "$hooks_dir"
  [ "$status" -eq 0 ]
  [ -f "$hooks_dir/git-gud" ]
  [ -L "$hooks_dir/post-checkout" ]
  [ -L "$hooks_dir/commit-msg" ]
  [ "$(readlink "$hooks_dir/post-checkout")" = "$(cd "$hooks_dir" && pwd -P)/git-gud" ]
  [ ! -e "$TEST_REPO/.git/hooks/git-gud" ]

  run "$GIT_GUD_BINARY" install "$hooks_dir"
  [ "$status" -eq 1 ]

  run "$GIT_GUD_BINARY" install --force "$hooks_dir"
  [ "$status" -eq 0 ]
  [ -L "$hooks_dir/post-checkout" ]
}

@test "install rejects extra directories" {
  mkdir "$TEST_REPO/custom-hooks"
  run "$GIT_GUD_BINARY" install "$TEST_REPO/custom-hooks" "$TEST_REPO/.git/hooks"
  [ "$status" -eq 1 ]
  [ ! -e "$TEST_REPO/custom-hooks/git-gud" ]
}
