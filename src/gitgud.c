#include "gitgud.h"
#include "util.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <git2.h>

#include "ginstall.h"
#include "gversion.h"

typedef enum {
  GUD_CMD_VERSION,
  GUD_CMD_INSTALL,

  GUD_CMD_INVALID = -1,
  GUD_CMD_BLANK = -2,
} gud_cmd_t;

static gud_cmd_t detect_gud_cmd(int argc, char **argv) {
  if (argc < 2) {
    return GUD_CMD_BLANK;
  }

  char *cmd = argv[1];

  if (strcmp(cmd, "version") == 0) {
    return GUD_CMD_VERSION;
  }

  if (strcmp(cmd, "install") == 0) {
    return GUD_CMD_INSTALL;
  }

  return GUD_CMD_INVALID;
}
/* options descriptor */
static struct option install_options[] = {{"force", no_argument, NULL, 'f'},
                                          {NULL, 0, NULL, 0}};

int git_gud_main(int argc, char **argv) {
  git_libgit2_init();
  int ch;

  gud_cmd_t cmd = detect_gud_cmd(argc, argv);
  getcwd(CWD, PATH_MAX);
  int status = 0;

  switch (cmd) {
  case GUD_CMD_VERSION:
    status = git_gud_version();
    break;

  case GUD_CMD_INSTALL:
    install_opts opts = {.force = false, .src = argv[0]};
    while ((ch = getopt_long(argc, argv, "f", install_options, NULL)) != -1) {
      switch (ch) {
      case 'f':
        opts.force = true;
        break;
      }
    }

    status = git_gud_install(opts);
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
