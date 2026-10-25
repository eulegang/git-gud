#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syslog.h>

#include "detect.h"
#include "gitgud.h"
#include "hooks/commit-msg.h"
#include "hooks/post-checkout.h"
#include "util.h"

int main(int argc, char **argv) {
  init_trace_syslog();

  cmd_t cmd = detect_cmd(argc, argv);
  int status = 0;

  const char *cwd = getcwd(CWD, PATH_MAX);
  if (!cwd) {
    perror("failed to find current working directory");
    return 1;
  }

  switch (cmd) {
  case CMD_GIT_GUD:
    syslog(LOG_DEBUG, "using git gud driver");
    status = git_gud_main(argc, argv);
    break;
  case CMD_POST_CHECKOUT:
    syslog(LOG_DEBUG, "using checkout driver");
    status = hook_post_checkout(argc, argv);
    break;
  case CMD_COMMIT_MSG:
    syslog(LOG_DEBUG, "using commit-msg driver");
    status = hook_commit_msg(argc, argv);
    break;

  case CMD_INVALID:
    die("failed to detect the type of script this is supposed to run\n");
    break;
  }

  deinit_trace_log();

  return status;
}
