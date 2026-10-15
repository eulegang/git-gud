#pragma once

typedef enum {
  CMD_GIT_GUD = 0,
  CMD_POST_CHECKOUT = 1,
  CMD_INVALID = -1,
} cmd_t;

cmd_t detect_cmd(int argc, char **argv);
