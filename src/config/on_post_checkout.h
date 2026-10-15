#pragma once

typedef struct config config_t;
typedef enum { CheckoutFile, CheckoutBranch } checkout_t;
typedef struct {
  const char *prev;
  const char *next;
  checkout_t checkout;
} post_checkout_t;

void config_post_checkout(config_t *, post_checkout_t);
