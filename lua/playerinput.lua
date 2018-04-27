ax = 0
az = 0
xm = 0
zm = 0
rotya = 0
rotxa = 0
speed = 0


turnBob = 0
headbob = 0
headturnBob = 0
headbobPhase = 0
walkspeed = 0.1 / 3.5




function handlecamerainput()
	yrotspeed = 0.04
	xrotspeed = 0.02
	movespeed = walkspeed

	xm = 0
	zm = 0

	if keys.W then zm = zm + 1 end
	if keys.S then zm = zm - 1 end
	if keys.D then xm = xm + 1 end
	if keys.A then xm = xm - 1 end
	if keys.LSHIFT then movespeed = movespeed * 2 end

	if keys.RIGHT then camera.roty = camera.roty + yrotspeed end
	if keys.LEFT then camera.roty = camera.roty - yrotspeed end

	if keys.UP then camera.rotx = camera.rotx - xrotspeed end
	if keys.DOWN then camera.rotx = camera.rotx + xrotspeed end

	dd = xm * xm + zm * zm
	if dd > 0 then
		dd = math.sqrt(dd)
	else
		dd = 1
	end

	xm = xm / dd
	zm = zm / dd

	headturnBob = headturnBob * 0.8
	headturnBob = headturnBob + rotya

	headbob = headbob * 0.6
	mmagnitude = math.sqrt(xm * xm + zm * zm)
	headbob = headbob + mmagnitude
	headbobPhase = headbobPhase + mmagnitude

	rCos = math.cos(camera.roty)
	rSin = math.sin(camera.roty)

	ax = ax + (xm * rCos + zm * rSin) * movespeed
	az = az + (zm * rCos - xm * rSin) * movespeed

	-- Collisions here
	tx = math.floor(camera.x);
	ty = math.floor(camera.z);
	r = 0.1

	newtx = math.floor(camera.x + ax + r * math.sign(ax))
	newty = math.floor(camera.z + az + r * math.sign(az))
	xb = getblock(newtx, ty)
	yb = getblock(tx, newty)
	if xb ~= 0 and xb ~= 0x00ff00 then
		-- ax = 0;
	end
	if yb ~= 0 and yb ~= 0x00ff00 then
		-- az = 0;
	end

	speed = math.sqrt(ax * ax + az * az)
	
	camera.x = camera.x + ax
	camera.z = camera.z + az
	-- math.sin(game.frames / 40) / 2
	camera.y = 0.6 + math.sin(headbobPhase * 0.4) * 0.01 * headbob;


	-- print(camera.x, camera.z)

	-- apply friction to acceleration
	ax = ax * 0.6;
	az = az * 0.6;

	-- Turn the camera
	camera.roty = camera.roty + rotya;
	-- camera.rotx = 45.0
	rotya = rotya * 0.4;

	camera.rotx = camera.rotx + rotxa;
	-- camera.rotx = 45.0
	rotxa = rotxa * 0.4;

	-- Clamp up and down rotation
	if camera.rotx > 1.25 then camera.rotx = 1.25 end
	if camera.rotx < -1.25 then camera.rotx = -1.25 end

	if camera.roty > math.pi * 2 then camera.roty = 0 end
	if camera.roty < 0 then camera.roty = camera.roty + math.pi * 2 end
end
