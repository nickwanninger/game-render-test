__camera__ = {}
setmetatable(__camera__, __camera__)
function __camera__.__call()
	local self = {}
	setmetatable(self, __camera__)
	return self
end
__camera__.__index = camera_index
__camera__.__newindex = camera_newindex




__game__ = {}
setmetatable(__game__, __game__)
function __game__.__call()
	local self = {}
	setmetatable(self, __game__)
	return self
end
__game__.__index = game_index
__game__.__newindex = game_newindex



-- GLOBAL DEFINITIONS:
camera = __camera__()
game = __game__()


-- Pre-define the game loop functions to protect against nil calls on the c side.
-- Not having these won't cause the program to crash, but it can cause errors
function init() end
function preupdate() end
function update() end
function drawworld() end
function drawgui() end
