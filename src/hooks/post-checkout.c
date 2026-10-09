#include "post-checkout.h"

#include <stdio.h>

int hook_post_checkout(int, char **) {
  printf("abc\n");
  return 0;
}
