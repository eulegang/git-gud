#include "detect.h"

#include <libgen.h>
#include <string.h>

cmd_t detect_cmd(int argc, char **argv) {
  if (argc < 1) {
    return CMD_INVALID;
  }

  char *name = basename(argv[0]);

  if (strcmp(name, "git-gud") == 0) {
    return CMD_GIT_GUD;
  }

  if (strcmp(name, "post-checkout") == 0) {
    return CMD_POST_CHECKOUT;
  }

  return CMD_INVALID;
}
