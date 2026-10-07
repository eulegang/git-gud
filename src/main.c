#include <stdio.h>

#include "detect.h"

int main(int argc, char **argv) {
  puts("Hello, git_gud!");

  cmd_t cmd = detect_cmd(argc, argv);

  switch (cmd) {
  case CMD_GIT_GUD:
    puts("GIT-GUD");
    break;
  case CMD_POST_CHECKOUT:
    puts("POST-CHECKOUT");
    break;
  case CMD_INVALID:
    puts("Invalid command given");
    break;
  }

  return 0;
}
