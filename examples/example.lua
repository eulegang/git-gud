--- @class Gud
--- @field on_checkout fun(handler: fun(checkout: Checkout))

--- @class Checkout
--- @field diff boolean
--- @field link fun(path: string)
--- @field copy fun(path: string)
--- @field shell fun(path: string)
--- @field exists fun(path: string): boolean
--- @field root_has fun(path: string): boolean
--- @field changes fun(path: string): boolean

--- @type Gud
local gud

gud.on_checkout(function(checkout)
	if ~checkout.diff and checkout.root_has(".envrc") then
		checkout.link(".envrc")
		checkout.shell("direnv allow .")
	end

	if checkout.exists("CMakeLists.txt") then
		checkout.shell("cmake -B build && cmake --build build")
	end
end)
