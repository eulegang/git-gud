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
#include "hooks/hooks.h"

char CWD[PATH_MAX];

typedef enum {
  GUD_CMD_VERSION = 0,
  GUD_CMD_INSTALL = 1,
  GUD_CMD_RUN = 2,

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

  if (strcmp(cmd, "run") == 0) {
    return GUD_CMD_RUN;
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
  int status = 0;

  switch (cmd) {
  case GUD_CMD_VERSION:
    if (argc > 3) {
      die("version accepts at most one directory\n");
    }
    status = git_gud_version(argc == 3 ? argv[2] : NULL);
    break;

  case GUD_CMD_INSTALL:
    install_opts opts = {.force = false, .src = argv[0], .directory = NULL};
    while ((ch = getopt_long(argc - 1, argv + 1, "f", install_options, NULL)) !=
           -1) {
      switch (ch) {
      case 'f':
        opts.force = true;
        break;
      default:
        die("invalid install option\n");
      }
    }

    if (optind < argc - 1) {
      opts.directory = argv[optind + 1];
    }
    if (optind < argc - 2) {
      die("install accepts at most one directory\n");
    }

    status = git_gud_install(opts);
    break;

  case GUD_CMD_RUN: {
    if (argc < 3) {
      die("run requires a hook name\n");
    }

    // Only hook drivers may be run; the git-gud driver is a command, not a hook.
    if (strcmp(argv[2], "post-checkout") != 0 &&
        strcmp(argv[2], "commit-msg") != 0) {
      die("unsupported hook '%s'\n", argv[2]);
    }

    const hook_t *hook = detect_registered_hook(argc - 2, argv + 2);
    status = hook->run(argc - 2, argv + 2);
    break;
  }

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
