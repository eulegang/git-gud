#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "detect.h"
#include "gitgud.h"
#include "util.h"

int main(int argc, char **argv) {
  cmd_t cmd = detect_cmd(argc, argv);

  switch (cmd) {
  case CMD_GIT_GUD:
    return git_gud_main(argc, argv);
    break;
  case CMD_POST_CHECKOUT:
    puts("POST-CHECKOUT");
    break;
  case CMD_INVALID:
    die("failed to detect the type of script this is supposed to run\n");
    break;
  }

  return 0;
}
