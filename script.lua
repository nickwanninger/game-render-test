require("lua/bindings")
require("lua/keymaps")
require("lua/playerinput")

ticks = 0
-- KEY_W = 26


function init()
	-- game.drawwalls = false
	-- game.drawfloor = not keys["2"]
	-- game.drawfog = false
	-- game.fogdist = 100
	
end

function update()
-- print("("..camera.x..", "..camera.y..")")
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
	handlecamerainput()
end




function drawworld()
	rCos = math.cos(camera.rot)
	rSin = -math.sin(camera.rot)

	dx = 0
	dy = 3

	xx = (dx * rCos - dy * rSin)
	yy =  (dy * rCos + dx * rSin)

	-- rendersprite(xx + camera.x, yy + camera.y, 0, 16, 16);
	rendersprite(3.5, 4.5, 0, 16, 16);
end


function onKeyDown(code)
	if code == keycodes.KEY_F3 then
		game.drawprofiler = not game.drawprofiler
	end
end
