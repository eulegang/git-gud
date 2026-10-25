#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syslog.h>

#include "hooks/hooks.h"
#include "gitgud.h"
#include "util.h"

int main(int argc, char **argv) {
  init_trace_syslog();

  const hook_t *hook = detect_registered_hook(argc, argv);

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
