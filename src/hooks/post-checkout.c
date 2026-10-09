#include "post-checkout.h"

#include "../util.h"

#include <stdio.h>

typedef enum { CheckoutFile, CheckoutBranch } checkout_t;

int hook_post_checkout(int argc, char **argv) {
  printf("abc\n");

  warn("args\n");
  for (int i = 0; i < argc; i++) {
    warn("- '%s'\n", argv[i]);
  }

  if (argc != 4) {
    die("invalid invocation");
  }

  const char *prev = argv[1];
  const char *next = argv[2];
  checkout_t checkout = *argv[3] == '0' ? CheckoutFile : CheckoutBranch;

  return 0;
}
