#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syslog.h>

#include "detect.h"
#include "gitgud.h"
#include "hooks/commit-msg.h"
#include "hooks/post-checkout.h"
#include "util.h"

static const hook_t hooks[] = {
    {"git-gud", git_gud_main},
    {"post-checkout", hook_post_checkout},
    {"commit-msg", hook_commit_msg},
};

int main(int argc, char **argv) {
  init_trace_syslog();

  const hook_t *hook =
      detect_hook(argc, argv, hooks, sizeof(hooks) / sizeof(hooks[0]));

  const char *cwd = getcwd(CWD, PATH_MAX);
  if (!cwd) {
    perror("failed to find current working directory");
    return 1;
  }

  if (!hook) {
    die("failed to detect the type of script this is supposed to run\n");
  }

  gud_log(LOG_DEBUG, "using %s driver\n", hook->name);
  int status = hook->run(argc, argv);

  deinit_trace_log();

  return status;
}
