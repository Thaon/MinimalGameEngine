// MinimalGameEngine.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Engine.h"
#include <LUA/sol.hpp>

const float gravity = .3;
float gSpeed = 0;

sol::state lua;

void UpdateState(Engine* eng)
{
	lua["delta"] = eng->deltaTime;

	//input
	lua["UpPressed"] = eng->inputPressed[UP];
	lua["UpHeld"] = eng->inputHeld[UP];
	lua["RightPressed"] = eng->inputPressed[RIGHT];
	lua["RightHeld"] = eng->inputHeld[RIGHT];
	lua["DownPressed"] = eng->inputPressed[DOWN];
	lua["DownHeld"] = eng->inputHeld[DOWN];
	lua["LeftPressed"] = eng->inputPressed[LEFT];
	lua["LeftHeld"] = eng->inputHeld[LEFT];
}

void Start(Engine* eng)
{
	//pass all the public methods
	lua.set_function("Log", &Engine::Log, eng);
	lua.set_function("FindEntity", &Engine::FindEntity, eng);
	lua.set_function("GetScene", &Engine::GetScene, eng);
	lua.set_function("Sign", sgn<float>);
	lua.set_function("PlaySound", &Engine::PlaySound, eng);
	lua.set_function("Quit", &Engine::QuitGame, eng);
	lua.set_function("LoadNextLevel", &Engine::LoadNextLevel, eng);
	lua.set_function("LoadLevel", &Engine::LoadLevel, eng);

	lua.set("Scene", eng->scene);
	lua.set("EntitySize", ENTITYSIZE);

	lua.script_file("resources/Game.lua");

	lua["Start"](eng);

	//eng->SetSpriteForEntity("Player", 0);
}

void Update(Engine* eng)
{
	//pass all public variables
	UpdateState(eng);
	
	lua["Update"](eng);
}

int main(int argc, char* args[])
{
	lua.open_libraries(sol::lib::base);

	Engine engine = Engine(&Start, &Update);

	//create classes
	lua.new_usertype<Entity>("Entity",

		//members
		"name", &Entity::name,
		"x", &Entity::x,
		"y", &Entity::y,
		"color", &Entity::color,
		"spriteIndex", &Entity::spriteIndex,
		"ID", &Entity::ID,

		//methods
		"Move", &Entity::Move,
		"SetSprite", &Entity::SetSprite,
		"TestCollision", &Entity::TestCollision,
		"TestCollisionBox", &Entity::TestCollisionBox,
		"TestCollisionPoint", &Entity::TestCollisionPoint
		);

	engine.Run();

	return 0;
}