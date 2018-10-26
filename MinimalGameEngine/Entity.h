#pragma once
#include <SDL/SDL.h>
#include <string>

static const int ENTITYSIZE = 32;

class Entity
{
public:
	Entity(std::string Name, float X, float Y, SDL_Color col)
	{
		name = Name;
		x = X;
		y = Y;
		color = col;
	}
	~Entity();

public:
	std::string name;
	float x, y;
	SDL_Color color;

	bool TestCollision(float dX, float dY, Entity* collider) //AABB swept collision check
	{
		if (x + dX + ENTITYSIZE/2 - 3 > collider->x - ENTITYSIZE/2 &&
			x + dX - ENTITYSIZE/2 + 3 < collider->x + ENTITYSIZE/2 && //x is inside
			y + dY + ENTITYSIZE/2 > collider->y - ENTITYSIZE/2 &&
			y + dY - ENTITYSIZE/2 < collider->y + ENTITYSIZE/2) //y is inside
			return true;
		else
			return false;
	}

	bool TestCollisionPoint(float X, float Y, Entity* collider)
	{
		if (x + X > collider->x - ENTITYSIZE / 2 &&
			x + X < collider->x + ENTITYSIZE / 2 && //x is inside
			y + Y > collider->y - ENTITYSIZE / 2 &&
			y + Y < collider->y + ENTITYSIZE / 2) //y is inside
			return true;
		else
			return false;
	}
private:

};

