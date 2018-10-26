#pragma once

#include <iostream>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <vector>
#include "Entity.h"
#include <fstream>

static const int UP = 0;
static const int RIGHT = 1;
static const int DOWN = 2;
static const int LEFT = 3;

//from: https://stackoverflow.com/questions/1903954/is-there-a-standard-sign-function-signum-sgn-in-c-c
template <typename T> int sgn(T val) {
	return (T(0) < val) - (val < T(0));
}

class Engine; //forward declaration
typedef void(*callbackType)(Engine*); //function pointer to make use of code external from engine

class Engine {
public:
	Engine(callbackType start, callbackType update)
	{
		startMethod = start;
		updateMethod = update;
	}

	~Engine()
	{

	}

public:
	//Screen dimension constants
	int SCREEN_WIDTH = ENTITYSIZE * 16;
	int SCREEN_HEIGHT = ENTITYSIZE * 16;

	//scene graph
	std::vector<Entity*>* scene;

	//input
	bool inputHeld[4];
	bool inputPressed[4];

	//utils
	float deltaTime = 0;

	void Log(char* msg)
	{
		std::cout << msg << std::endl;
	}

	void Run()
	{
		if (Init())
		{
			isRunning = true;
			AcquireResources();

			while (isRunning)
			{
				float time = SDL_GetTicks() / 1000.0; //SDL_GetTicks returns a Uint32 representing the number of milliseconds between frames, needs to be transposed in actual fractions of seconds
				deltaTime = time - lastTime;
				ProcessInput();
				Update();
				RenderBackground();
				Render();
				lastTime = time;
				ReleaseInputs();
			}

			Terminate();
		}
		else
		{
			std::cout << "something is wrong" << std::endl;
			system("PAUSE");
		}
	}

	Entity* AddEntity(std::string name, float x, float y, SDL_Color col)
	{
		Entity* ent = new Entity(name, x, y, col);
		scene->push_back(ent);
		return ent;
	}

	Entity* FindEntity(std::string name)
	{
		for (Entity* e : *scene)
		{
			if (e->name == name)
				return e;
		}
		return NULL;
	}

	void QuitGame()
	{
		isRunning = false;
	}

	void LoadNextLevel()
	{
		//check if level exists
		std::string fName = "resources/" + std::to_string(currentLevel) + ".bmp";
		std::ifstream f(fName.c_str());
		if (f.good())
		{
			std::cout << "Loading " << fName << std::endl;
			currentLevelSurface = IMG_Load(fName.c_str());
			currentLevel++;
		}
		else //if it doesn't exist, we completed all the levels
		{
			Log("You Won!");
			isRunning = false;
			return;
		}
		//we gucci, clear scene
		scene->clear();

		//call Start
		startMethod(this);

		//if it does, let's generate a level from it!
		for (int i = 0; i < currentLevelSurface->w; i++)
		{
			for (int j = 0; j < currentLevelSurface->h; j++)
			{

				Uint8 red, green, blue;
				SDL_GetRGB(getpixel(currentLevelSurface, i, j), currentLevelSurface->format, &red, &green, &blue);

				if (red == 255 && green == 0 && blue == 0)
				{
					AddEntity("Enemy", i * ENTITYSIZE, j * ENTITYSIZE, { red, green, blue });
				}
				if (red == 0 && green == 0 && blue == 255)
				{
					AddEntity("Goal", i * ENTITYSIZE, j * ENTITYSIZE, { red, green, blue });
				}
				if (red == 64 && green == 64 && blue == 64)
				{
					AddEntity("Wall", i * ENTITYSIZE, j * ENTITYSIZE, { red, green, blue });
				}
				if (red == 0 && green == 255 && blue == 0)
				{
					AddEntity("Player", i * ENTITYSIZE, j * ENTITYSIZE, { red, green, blue });
				}
			}
		}
	}

	int GetCurrentLevel()
	{
		return currentLevel;
	}

private:
	//The window we'll be rendering to
	SDL_Window* window = NULL;

	//The surface contained by the window
	SDL_Surface* screenSurface = NULL;
	SDL_Surface* background = NULL;

	//font
	TTF_Font *font = 0;

	//gameplay stuff
	bool isRunning = false;
	callbackType startMethod;
	callbackType updateMethod;

	//levels
	int currentLevel = 0;
	SDL_Surface* currentLevelSurface = NULL;

	//delta time calculation
	float lastTime;

	//Resources utilities
	bool LoadBackGround()
	{
		background = IMG_Load("resources/background.png");

		if (!background)
			return false;
		else
			return true;
	}

	bool LoadFont()
	{
		font = TTF_OpenFont("resources/slkscr.ttf", 12);

		if (!font)
			return false;
		else
			return true;
	}

	void AcquireResources()
	{
		if (!LoadBackGround())
		{
			std::cout << "Could not load Background :(" << std::endl;
			isRunning = false;
		}

		if (!LoadFont())
		{
			std::cout << "Could not load Font :(" << std::endl;
			isRunning = false;
		}

		LoadNextLevel(); //load here?
	}


	Uint32 getpixel(SDL_Surface *surface, int x, int y)
	{
		SDL_LockSurface(surface);
		int bpp = surface->format->BytesPerPixel;
		/* Here p is the address to the pixel we want to retrieve */
		Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

		switch (bpp) {
		case 1:
			SDL_UnlockSurface(surface);
			return *p;
			break;

		case 2:
			SDL_UnlockSurface(surface);
			return *(Uint16 *)p;
			break;

		case 3:
			if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
			{
				SDL_UnlockSurface(surface);
				return p[0] << 16 | p[1] << 8 | p[2];
			}
			else
			{
				SDL_UnlockSurface(surface);
				return p[0] | p[1] << 8 | p[2] << 16;
			}
			break;

		case 4:
			SDL_UnlockSurface(surface);
			return *(Uint32 *)p;
			break;

		default:
			SDL_UnlockSurface(surface);
			return 0;       /* shouldn't happen, but avoids warnings */
		}
	}
	//Window management

	bool Init()
	{
		//Initialize SDL
		if (SDL_Init(SDL_INIT_VIDEO) < 0)
		{
			std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
			return false;
		}
		else
		{
			//Create window
			window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
			if (window == NULL)
			{
				std::cout << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
				return false;
			}
			else
			{
				//Get window surface
				screenSurface = SDL_GetWindowSurface(window);

				//Fill the surface white
				SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));

				//Update the surface
				SDL_UpdateWindowSurface(window);

				//Initialize fonts
				TTF_Init();

				//init scene
				scene = new std::vector<Entity*>();

				return true;
			}
		}
	}

	void Terminate()
	{
		//Destroy window
		SDL_DestroyWindow(window);

		//free surfaces
		SDL_FreeSurface(background);

		//free font
		if (font)
			TTF_CloseFont(font);
		font = 0;

		//Quit SDL subsystems
		SDL_Quit();

		//clear scene
		delete scene;

		std::cout << "Game terminated" << std::endl;
		system("PAUSE");
	}

	//main engine functions
	void RenderBackground()
	{
		//draw background
		SDL_BlitSurface(background, 0, screenSurface, 0);
	}

	void Render()
	{
		for (Entity* e : *scene)
		{
			const SDL_Rect Rect = { e->x, e->y, ENTITYSIZE, ENTITYSIZE }; //our entities will always be 32 by 32
			Uint32 col = SDL_MapRGB(screenSurface->format, e->color.r, e->color.g, e->color.b);
			SDL_FillRect(screenSurface, &Rect, col);
			//SDL_BlitSurface(background, 0, screenSurface, 0);
		}

		//update screen
		SDL_UpdateWindowSurface(window);
	}

	void Update()
	{
		updateMethod(this);
	}

	void ProcessInput()
	{
		//Event handler
		SDL_Event e;

		//Handle events on queue
		SDL_PollEvent(&e);

		//User requests quit
		if (e.type == SDL_QUIT)
		{
			isRunning = false;
		}
	//User presses a key
		else if (e.type == SDL_KEYDOWN)
		{
			//Select surfaces based on key press
			if (e.key.keysym.sym == SDLK_UP)
			{
				if(!inputHeld[UP])
					inputPressed[UP] = true;

				inputHeld[UP] = true;
			}
			if (e.key.keysym.sym == SDLK_RIGHT)
			{
				if (!inputHeld[RIGHT])
					inputPressed[RIGHT] = true;

				inputHeld[RIGHT] = true;
			}
			if (e.key.keysym.sym == SDLK_DOWN)
			{
				if (!inputHeld[DOWN])
					inputPressed[DOWN] = true;
	
				inputHeld[DOWN] = true;
			}
			if (e.key.keysym.sym == SDLK_LEFT)
			{
				if (!inputHeld[LEFT])
					inputPressed[LEFT] = true;
	
				inputHeld[LEFT] = true;
			}
			if (e.key.keysym.sym == SDLK_ESCAPE)
			{
				isRunning = false;
			}
		}
		else if (e.type == SDL_KEYUP)
		{
			//Select surfaces based on key press
			if (e.key.keysym.sym == SDLK_UP)
			{
				inputHeld[UP] = false;
			}
			if (e.key.keysym.sym == SDLK_RIGHT)
			{
				inputHeld[RIGHT] = false;
			}if (e.key.keysym.sym == SDLK_DOWN)
			{
				inputHeld[DOWN] = false;
			}if (e.key.keysym.sym == SDLK_LEFT)
			{
				inputHeld[LEFT] = false;
			}
		}
	}

	void ReleaseInputs()
	{
		inputPressed[UP] = false;
		inputPressed[RIGHT] = false;
		inputPressed[DOWN] = false;
		inputPressed[LEFT] = false;
	}
};
