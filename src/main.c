#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "detect.h"
#include "gitgud.h"
#include "hooks/post-checkout.h"
#include "util.h"

int main(int argc, char **argv) {
  const char *tracefile = getenv("GIT_GUD_TRACE_FILE");
  if (tracefile) {
    if (tracefile[0] == 0) {
      init_trace_log(NULL);
    } else {
      init_trace_log(tracefile);
    }
  }

  when_tracing {
    trace("starting git-gud\n");
    for (int i = 0; i < argc; i++) {
      trace("  - %s\n", argv[0]);
    }
  }

  cmd_t cmd = detect_cmd(argc, argv);
  int status = 0;

  switch (cmd) {
  case CMD_GIT_GUD:
    status = git_gud_main(argc, argv);
    break;
  case CMD_POST_CHECKOUT:
    return hook_post_checkout(argc, argv);
    break;
  case CMD_INVALID:
    die("failed to detect the type of script this is supposed to run\n");
    break;
  }

  deinit_trace_log();

  return status;
}
