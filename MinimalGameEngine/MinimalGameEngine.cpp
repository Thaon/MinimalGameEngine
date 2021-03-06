// MinimalGameEngine.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Engine.h"

const float gravity = .3;
float gSpeed = 0;

void Start(Engine* eng)
{
	if (eng->GetCurrentLevel() == 1)
		eng->Log("Welcome to crazy jumpman adventures, developed in the Minimal Game Engine!");

	if (eng->GetCurrentLevel() == 2)
		eng->Log("You can do it!!");

	//eng->SetSpriteForEntity("Player", 0);
}

void Update(Engine* eng)
{
	float dt = eng->deltaTime;
	float speed = 1;
	float jumpSpeed = -7;
	float hSpeed = 0, vSpeed = 0;
	//std::cout << eng->deltaTime << std::endl;

	Entity* ent = eng->FindEntity("Player");
	Entity* goal = eng->FindEntity("Exit");
	
	//input
	if (eng->inputHeld[RIGHT])
	{
		hSpeed += speed * dt;
	}
	/*if (eng->input[DOWN])
	{
		vSpeed += speed * dt;
	}*/
	if (eng->inputHeld[LEFT])
	{
		hSpeed -= speed * dt;
	}

	bool grounded = false;
	//check for wall collisions
	for (Entity* wall : *eng->scene)
	{
		if (wall->name == "Wall")
		{
			//boudaries check
			if (ent->TestCollisionPoint((ENTITYSIZE / 2) * sgn(hSpeed) + hSpeed, 0, wall))
			{
				ent->x -= hSpeed;
			}
			//ceiling check
			if (vSpeed < 0 && ent->TestCollisionPoint(0, (ENTITYSIZE / 2) + vSpeed, wall))
			{
				ent->y -= vSpeed;
				vSpeed = -vSpeed; //bounce!
			}
			//ground check
			else if (vSpeed >= 0 && ent->TestCollisionBox(ent->x, ent->y + vSpeed, ENTITYSIZE / 2 - 3, ENTITYSIZE / 2, wall))
			{
				ent->y -= vSpeed;
				grounded = true;
			}
		}
	}
	//apply gravity
	if (!grounded)
	{
		gSpeed += gravity * gravity * eng->deltaTime;
	}
	else
	{
		gSpeed = 0;
		ent->y -= vSpeed;
		//eng->Log("Collision with Wall!");

		//jump if grounded
		if (eng->inputPressed[UP])
		{
			gSpeed = jumpSpeed * eng->deltaTime;
			grounded = false;
			eng->PlaySound(0);
		}
	}

	//movement
	ent->x += hSpeed;
	ent->y += vSpeed + gSpeed;


	//collision check
	for (Entity* enemy : *eng->scene)
	{
		if (enemy->name == "Spikes")
		{
			if (ent->TestCollision(0, 0, enemy))
			{
				eng->QuitGame();
			}
		}
	}

	if (ent->TestCollision(0, 0, goal))
	{
		eng->LoadNextLevel();
	}
}

int main(int argc, char* args[])
{
	Engine engine = Engine(&Start, &Update);
	engine.Run();

	return 0;
}