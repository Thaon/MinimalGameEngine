#pragma once
#include <SDL/SDL.h>
#include <string>

static const int ENTITYSIZE = 32;

struct Entity
{
public:
	Entity(std::string Name, float X, float Y, SDL_Color col, int sprIndex)
	{
		name = Name;
		x = X;
		y = Y;
		color = col;
		spriteIndex = sprIndex;
	}

	Entity(std::string Name, float X, float Y, SDL_Color col)
	{
		name = Name;
		x = X;
		y = Y;
		color = col;
		spriteIndex = -1;
	}
	
	Entity(std::string Name, float X, float Y, int sprIndex)
	{
		name = Name;
		x = X;
		y = Y;
		color = *(new SDL_Color());
		spriteIndex = sprIndex;
	}

	Entity(std::string Name, SDL_Color col, int sprIndex)
	{
		name = Name;
		color = col;
		spriteIndex = sprIndex;
	}

public:
	std::string name = "";
	float x = 0, y = 0;
	SDL_Color color = *(new SDL_Color());
	int spriteIndex = -1;
	int drawOrder = 0;
	bool visible = true;
	int currentFrame = 0;
	bool animate = true;
	float animationTime = 0;
	int animationSpeed = 1; //ranges from 0 to 100
	bool toBeDestroyed = false;

	//properties

	void Move(float dX, float dY)
	{
		x += dX;
		y += dY;
	}

	void SetColor(Uint8 r, Uint8 g, Uint8 b)
	{
		color = { r, g, b };
	}

	void SetSprite(int index)
	{
		spriteIndex = index;
	}

	//collision

	bool TestCollision(float dX, float dY, Entity* collider) //AABB swept collision sprite check
	{
		if (x + dX + ENTITYSIZE/2 - 3 > collider->x - ENTITYSIZE/2 &&
			x + dX - ENTITYSIZE/2 + 3 < collider->x + ENTITYSIZE/2 && //x is inside
			y + dY + ENTITYSIZE/2 > collider->y - ENTITYSIZE/2 &&
			y + dY - ENTITYSIZE/2 < collider->y + ENTITYSIZE/2) //y is inside
			return true;
		else
			return false;
	}

	bool TestCollisionBox(float X, float Y, float width, float height, Entity* collider) //AABB swept collision box check
	{
		if (X + width > collider->x - ENTITYSIZE / 2 &&
			X - width < collider->x + ENTITYSIZE / 2 && //x is inside
			Y + height > collider->y - ENTITYSIZE / 2 &&
			Y - height < collider->y + ENTITYSIZE / 2) //y is inside
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

bool sortOrder(Entity* i, Entity* j) { return (i->drawOrder < j->drawOrder); }
