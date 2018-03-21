require("lua/bindings")
require("lua/keymaps")
require("lua/playerinput")

ticks = 0
-- KEY_W = 26


function init()
	-- game.drawwalls = false
	-- game.drawfloor = not keys["2"]
	-- game.drawfog = false
	
end

function update()
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

function preupdate()
	-- game.drawprofiler = keys.F3
	

	handlecamerainput()
end




function draw()
end


function onKeyDown(code)
	if code == keycodes.KEY_F3 then
		game.drawprofiler = not game.drawprofiler
	end
end
