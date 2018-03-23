require("lua/bindings")
require("lua/keymaps")
require("lua/playerinput")

ticks = 0
-- KEY_W = 26


function init()
	-- game.drawwalls = false
	-- game.drawfloor = false
	-- game.drawfog = false
	-- game.fogdist = 100
	game.renderdist = 10;
	
end

function math.sign(x)
	if x<0 then
		return -1
	elseif x>0 then
		return 1
	else
		return 0
	end
end

function update()
	handlecamerainput()
end




function drawworld()
	-- for x = 1, 6 do
	-- 	for y = 1, 6 do
	-- 		rendersprite(x + 0.5, y + 0.5, 0, 16, 0);
	-- 	end
	-- end
end


function onKeyDown(code)
	if code == keycodes.KEY_F3 then
		game.drawprofiler = not game.drawprofiler
	end
end
