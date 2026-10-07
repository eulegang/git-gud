#ifndef __DETECT_H
#define __DETECT_H

typedef enum {
  CMD_GIT_GUD,
  CMD_POST_CHECKOUT,
  CMD_INVALID = -1,
} cmd_t;

cmd_t detect_cmd(int argc, char **argv);
#endif
