--cache player and exit
local player
local exit

local speed = 1
local jumpSpeed = -7
local gravity = .3
local gSpeed = 0;

function Start(ME)
	player = FindEntity("Player")
	exit = FindEntity("Exit")
end

function Update(ME)
	--cache scene
	scene = GetScene()

	--movement variables
	hSpeed = 0
	vSpeed = 0;

	--process left and right movement
	if RightHeld then
		hSpeed = hSpeed + speed * delta;
	end
	
	if LeftHeld then
		hSpeed = hSpeed - speed * delta
	end

	if APressed then
		RestartLevel()
	end

	grounded = false

	--check for wall collisions
	for i,wall in ipairs(scene) do
		if wall.name == "Wall" then
			--sides check
			if player:TestCollisionPoint((EntitySize / 2) * Sign(hSpeed) + hSpeed, 0, wall) then
				player.x = player.x - hSpeed
				RemoveEntity(wall)

			--ground check
			elseif vSpeed >= 0 and player:TestCollisionBox(player.x, player.y + vSpeed, EntitySize / 2 - 3, EntitySize / 2, wall) then
				player.y = player.y - vSpeed
				grounded = true
			end
		end
	end

	--apply gravity
	if not grounded then
		gSpeed = gSpeed + gravity * gravity * delta
	else
		gSpeed = 0
		player.y = player.y - vSpeed

		--jump if grounded
		if UpPressed then
			gSpeed = jumpSpeed * delta
			grounded = false
			PlaySound(0)
		end
	end

	--apply movement to player
	player:Move(hSpeed, vSpeed + gSpeed)

	--hazards collision check
	for i,hazard in ipairs(scene) do
		if hazard.name == "Spikes" then
			if player:TestCollision(0, 0, hazard) then
				Quit();
			end
		end
	end

	if player:TestCollision(0, 0, exit) then
		LoadNextLevel()
	end
end
