#include "post-checkout.h"

#include "../gitgud.h"
#include "../util.h"

#include "../config/config.h"

#include <stdio.h>

int hook_post_checkout(int argc, char **argv) {
  printf("abc\n");

  warn("args\n");
  for (int i = 0; i < argc; i++) {
    warn("- '%s'\n", argv[i]);
  }

  if (argc != 4) {
    die("invalid invocation");
  }

  post_checkout_t data;
  data.prev = argv[1];
  data.next = argv[2];
  data.checkout = *argv[3] == '0' ? CheckoutFile : CheckoutBranch;

  git_repository *repo;
  int status = git_repository_open_ext(&repo, CWD, 0, NULL);
  if (status == -1) {
    auto err = git_error_last();

    die("failed to open git repo: %s\n", err->message);
  }

  config_t *config = config_init(config_path(), repo);
  if (!config) {
    die("failed to read config");
  }

  config_post_checkout(config, data);

  config_deinit(config);

  return 0;
}
