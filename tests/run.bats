#!/usr/bin/env bats

setup() {
  test -n "${GIT_GUD_BINARY:-}"
  test -x "$GIT_GUD_BINARY"

  TEST_REPO="$(mktemp -d)"
  git -C "$TEST_REPO" init --quiet
  cd "$TEST_REPO"
  export GIT_GUD_INIT="$TEST_REPO/init.lua"
  cat > "$GIT_GUD_INIT" <<'LUA'
gud.on_commit_msg(function(event)
  assert(event.path == "message.txt")
  assert(event.subject == "test subject")
  local file = assert(io.open("commit-ran", "w"))
  file:write(event.content)
  file:close()
end)

gud.on_checkout(function(event)
  assert(event.prev == "before")
  assert(event.next == "after")
  assert(event.is_branch)
  local file = assert(io.open("checkout-ran", "w"))
  file:write("yes")
  file:close()
end)
LUA
}

teardown() {
  rm -rf "$TEST_REPO"
}

@test "run commit-msg forwards the message path" {
  printf 'test subject\n' > message.txt
  run "$GIT_GUD_BINARY" run commit-msg message.txt
  [ "$status" -eq 0 ]
  [ "$(cat commit-ran)" = "test subject" ]
  [ ! -e checkout-ran ]
}

@test "run post-checkout forwards checkout arguments" {
  run "$GIT_GUD_BINARY" run post-checkout before after 1
  [ "$status" -eq 0 ]
  [ "$(cat checkout-ran)" = "yes" ]
  [ ! -e commit-ran ]
}

@test "run propagates hook failure" {
  printf 'fail("rejected")\n' > "$GIT_GUD_INIT"
  printf 'test subject\n' > message.txt
  run "$GIT_GUD_BINARY" run commit-msg message.txt
  [ "$status" -ne 0 ]
  [[ "$output" == *"rejected"* ]]
}

@test "run rejects missing or unsupported hooks" {
  run "$GIT_GUD_BINARY" run
  [ "$status" -ne 0 ]
  run "$GIT_GUD_BINARY" run git-gud version
  [ "$status" -ne 0 ]
  run "$GIT_GUD_BINARY" run unknown
  [ "$status" -ne 0 ]
  [ ! -e commit-ran ]
  [ ! -e checkout-ran ]
}

@test "run rejects wrong hook argument counts" {
  run "$GIT_GUD_BINARY" run commit-msg
  [ "$status" -ne 0 ]
  run "$GIT_GUD_BINARY" run post-checkout before after
  [ "$status" -ne 0 ]
}
