#include "on_commit.h"
#include "internal.h"
#include <string.h>
#include <sys/syslog.h>

#include "../commit/commit.h"
#include "../util.h"

void config_on_commit_msg(config_t *config, commit_msg_t commit_msg) {
  lua_newtable(config->state);

  lua_pushstring(config->state, commit_msg.path);
  lua_setfield(config->state, -2, "path");

  lua_pushstring(config->state, commit_msg.content);
  lua_setfield(config->state, -2, "content");

  message_t *msg = message_parse(commit_msg.content);

  lua_pushstring(config->state, msg->subject);
  lua_setfield(config->state, -2, "subject");

  if (msg->description) {
    lua_pushstring(config->state, msg->description);
  } else {
    lua_pushnil(config->state);
  }
  lua_setfield(config->state, -2, "body");

  lua_newtable(config->state);

  message_trailer_iter_t iter = MESSAGE_TRAILER_ITER_INIT;
  const char *key = NULL, *value = NULL;

  while (message_trailer_next(msg, &iter, &key, &value)) {
    lua_pushstring(config->state, key);
    lua_pushstring(config->state, value);

    lua_settable(config->state, -3);
  }

  lua_setfield(config->state, -2, "trailers");

  for (int i = 0; i < config->on_commit_msg.n; i++) {
    int ref = config->on_commit_msg.refs[i];

    lua_rawgeti(config->state, LUA_REGISTRYINDEX, ref);
    lua_pushvalue(config->state, -2);

    if (lua_pcall(config->state, 1, 0, 0) != LUA_OK) {
      const char *err = lua_tostring(config->state, -1);
      syslog(LOG_ERR, "failed to call on_commit: %s", err);
      (void)fprintf(stderr, "failed to call on_commit: %s\n", err);

      if (config->status & STATUS_FAIL) {
        exit(16);
      }
    }
  }

  lua_pop(config->state, 1);
}
