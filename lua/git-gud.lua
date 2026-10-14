---@meta git-gud

---Lua API exposed to git-gud configuration files.
---
---The `gud` table is available as a global in the file loaded from
---`~/.config/git/gud.lua`, `~/.config/git-gud/init.lua`, or `GIT_GUD_INIT`.
---@class GitGud
---@field on_checkout fun(handler: fun(checkout: GitGud.CheckoutEvent)) Register a handler for Git's post-checkout hook.

---Data and helpers passed to `gud.on_checkout` handlers.
---@class GitGud.CheckoutEvent
---@field next string The commit checked out.
---@field prev string The previously checked out commit.
---@field is_branch boolean True when the checkout was a branch checkout.
---@field is_file boolean True when the checkout was a file checkout.
---@field is_new_worktree boolean True when the checkout created a new worktree.
---@field link fun(path: string): boolean Hard-link `path` from the parent repository into the current worktree.
---@field exists fun(path: string): boolean True when `path` exists in the current worktree.
---@field root_has fun(path: string): boolean True when `path` exists in the parent repository worktree.
---@field changes fun(path: string): boolean True when `path` changed between `prev` and `next`.

---@type GitGud
gud = {}

---Write a message to git-gud's trace output when tracing is enabled.
---@param message string
function trace(message) end
