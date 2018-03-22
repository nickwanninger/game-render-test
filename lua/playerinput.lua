ax = 0
ay = 0
xm = 0
ym = 0
rota = 0
speed = 0


turnBob = 0
headbob = 0
headturnBob = 0
headbobPhase = 0
walkspeed = 0.1 / 3.5




function handlecamerainput()
	rotspeed = 0.04
	movespeed = walkspeed

	xm = 0
	ym = 0

	if keys.W then ym = ym - 1 end
	if keys.S then ym = ym + 1 end
	if keys.D then xm = xm - 1 end
	if keys.A then xm = xm + 1 end
	if keys.LSHIFT then movespeed = movespeed * 2 end

	if keys.RIGHT then rota = rota + rotspeed end
	if keys.LEFT then rota = rota - rotspeed end

	dd = xm * xm + ym * ym
	if dd > 0 then
		dd = math.sqrt(dd)
	else
		dd = 1
	end

	xm = xm / dd
	ym = ym / dd

	headturnBob = headturnBob * 0.8
	headturnBob = headturnBob + rota

	headbob = headbob * 0.6
	mmagnitude = math.sqrt(xm * xm + ym * ym)
	headbob = headbob + mmagnitude
	headbobPhase = headbobPhase + mmagnitude

	rCos = math.cos(camera.rot)
	rSin = math.sin(camera.rot)

	ax = ax - (xm * rCos + ym * rSin) * movespeed
	ay = ay - (ym * rCos - xm * rSin) * movespeed

	-- Collisions here
	tx = math.floor(camera.x);
	ty = math.floor(camera.y);
	r = 0.1

	newtx = math.floor(camera.x + ax + r * math.sign(ax))
	newty = math.floor(camera.y + ay + r * math.sign(ay))
	xb = getblock(newtx, ty)
	yb = getblock(tx, newty)
	if xb ~= 0 and xb ~= 0x00ff00 then
		ax = 0;
	end
	if yb ~= 0 and yb ~= 0x00ff00 then
		ay = 0;
	end

	speed = math.sqrt(ax * ax + ay * ay)
	camera.x = camera.x + ax
	camera.y = camera.y + ay
	-- math.sin(game.frames / 40) / 2
	camera.z = 0.2 + math.sin(headbobPhase * 0.4) * 0.01 * headbob;

	-- apply friction to acceleration
	ax = ax * 0.6;
	ay = ay * 0.6;

	-- Turn the camera
	camera.rot = camera.rot + rota;
	rota = rota * 0.4;

	if camera.rot > math.pi * 2 then camera.rot = 0 end
	if camera.rot < 0 then camera.rot = camera.rot + math.pi * 2 end
end
