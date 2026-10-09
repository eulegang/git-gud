#ifndef __CONFIG_H
#define __CONFIG_H

typedef struct config config_t;

config_t *config_init(const char *path);
void config_deinit(config_t *);

#endif
