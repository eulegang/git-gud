#include "gitgud.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <git2.h>

#include "gversion.h"

typedef enum {
  GUD_CMD_VERSION,
  GUD_CMD_BLANK,
  GUD_CMD_INVALID = -1,
} gud_cmd_t;

static gud_cmd_t detect_gud_cmd(int argc, char **argv) {
  if (argc < 2) {
    return GUD_CMD_BLANK;
  }

  char *cmd = argv[1];

  if (strcmp(cmd, "version") == 0) {
    return GUD_CMD_VERSION;
  }

  return GUD_CMD_INVALID;
}

int git_gud_main(int argc, char **argv) {
  git_libgit2_init();

  gud_cmd_t cmd = detect_gud_cmd(argc, argv);
  getcwd(CWD, PATH_MAX);
  int status = 0;

  switch (cmd) {
  case GUD_CMD_VERSION:
    status = git_gud_version();
    break;

  case GUD_CMD_INVALID:
    die("invalid command issues '%s'\n", argv[1]);
    break;

  case GUD_CMD_BLANK:
    die("no command given\n");
    break;
  }

  git_libgit2_shutdown();

  return status;
}
