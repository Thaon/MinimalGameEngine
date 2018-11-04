// MinimalGameEngine.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Engine.h"
#include <windows.h>
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
	lua["StartPressed"] = eng->inputPressed[START];
	lua["StartHeld"] = eng->inputHeld[START];
	lua["APressed"] = eng->inputPressed[A];
	lua["AHeld"] = eng->inputHeld[A];
	lua["BPressed"] = eng->inputPressed[B];
	lua["BHeld"] = eng->inputHeld[B];
}

void Start(Engine* eng)
{
	//pass all the public methods
	lua.set_function("Log", &Engine::Log, eng);
	lua.set_function("Clear", &Engine::Clear, eng);
	lua.set_function("AddEntity", &Engine::AddEntity, eng);
	lua.set_function("RemoveEntity", &Engine::RemoveEntity, eng);
	lua.set_function("FindEntity", &Engine::FindEntity, eng);
	lua.set_function("GetScene", &Engine::GetScene, eng);
	lua.set_function("Sign", sgn<float>);
	lua.set_function("PlaySound", &Engine::PlaySDLSound, eng);
	lua.set_function("RestartLevel", &Engine::Restart, eng);
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

	HWND consoleWindow = GetConsoleWindow();
	SetWindowPos(consoleWindow, 0, 590, 800, 750, 200, SWP_NOZORDER);

	std::string filename = std::string(args[0]);
	int index = filename.find_last_of("/\\");
	std::string input_trace_filename = filename.substr(index + 1);

	Engine engine = Engine(&Start, &Update, input_trace_filename, true);

	//create classes
	lua.new_usertype<Entity>("Entity",

		//members
		"name", &Entity::name,
		"x", &Entity::x,
		"y", &Entity::y,
		"color", &Entity::color,
		"spriteIndex", &Entity::spriteIndex,
		"drawOrder", &Entity::drawOrder,
		"animate", &Entity::animate,
		"animationSpeed", &Entity::animationSpeed,
		"visible", &Entity::visible,

		//methods
		"Move", &Entity::Move,
		"SetColor", &Entity::SetColor,
		"SetSprite", &Entity::SetSprite,
		"TestCollision", &Entity::TestCollision,
		"TestCollisionBox", &Entity::TestCollisionBox,
		"TestCollisionPoint", &Entity::TestCollisionPoint
		);

	engine.Run();

	return 0;
}