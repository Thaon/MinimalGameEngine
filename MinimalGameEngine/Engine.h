#pragma once

#include <iostream>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>
#include <vector>
#include <algorithm>
#include "Entity.h"
#include <fstream>

static const int UP = 0;
static const int RIGHT = 1;
static const int DOWN = 2;
static const int LEFT = 3;
static const int START = 4;
static const int A = 5;
static const int B = 6;


//returns the sign of a number
//from: https://stackoverflow.com/questions/1903954/is-there-a-standard-sign-function-signum-sgn-in-c-c
template <typename T> int sgn(T val) {
	return (T(0) < val) - (val < T(0));
}

class Engine; //forward declaration needed to define callback
typedef void(*callbackType)(Engine*); //function pointer to make use of code external from engine

class Engine {
public:
	Engine(callbackType start, callbackType update, std::string Name, bool lite)
	{
		startMethod = start;
		updateMethod = update;
		name = Name;
		liteMode = lite;
	}

	~Engine()
	{

	}

public:
	//Screen dimension constants
	int SCREEN_WIDTH = ENTITYSIZE * 16;
	int SCREEN_HEIGHT = ENTITYSIZE * 16;

	//scene graph
	std::vector<Entity*>* entityesDB;
	std::vector<Entity*>* scene;

	//input
	bool inputHeld[7];
	bool inputPressed[7];

	//utils
	double deltaTime = 0;
	bool liteMode = false;

	//graphics
	std::vector<SDL_Surface*>* sprites;

	//music and sounds
	std::vector<Mix_Chunk*>* audioClips; //effects can be created using: https://jfxr.frozenfractal.com/

	std::string name;
	//MAIN ENGINE METHODS, ACCESSIBLE EXTERNALLY-----------------------------------------------------------------------------------------------------------------------------------

	//core engine cycle
	void Run()
	{
		if (Init(name))
		{
			isRunning = true;
			AcquireResources();

			//improved delta time calculation from: https://gamedev.stackexchange.com/questions/110825/how-to-calculate-delta-time-with-sdl/123957
			Uint64 NOW = SDL_GetPerformanceCounter();
			Uint64 LAST = 0;

			while (isRunning)
			{
				RemovedDestroyedEntities();
				ProcessInput();
				Update();
				LAST = NOW;
				NOW = SDL_GetPerformanceCounter();
				deltaTime = (double)((NOW - LAST) * 100 / (double)SDL_GetPerformanceFrequency()); //modified to 100 from 1000 to make it milliseconds
				Render();
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

	//adds an entity to the scene
	Entity* AddEntity(std::string name, float x, float y, int r, int g, int b, int spriteindex)
	{
		SDL_Color col{ r,g,b };
		Entity* ent = new Entity(name, x, y, col, spriteindex);
		scene->push_back(ent);
		return ent;
	}

	//marks an entity for removal
	void RemoveEntity(Entity* toRemove)
	{
		toRemove->toBeDestroyed = true;
	}

	//returns the first entity with a given name
	Entity* FindEntity(std::string name)
	{
		for (Entity* e : *scene)
		{
			if (e->name == name)
				return e;
		}
		return NULL;
	}

	//returns all the entities by value
	std::vector<Entity*>* GetScene()
	{
		return scene;
	}

	//sets the sprite of a given entity
	void SetSpriteForEntity(std::string Name, int index)
	{
		//change in the DB
		for (Entity* e : *entityesDB)
		{
			if (e->name == Name)
				e->SetSprite(index);
		}

		//change in the scene
		for (Entity* e : *scene)
		{
			if (e->name == Name)
				e->SetSprite(index);
		}
	}

	//cleans the scene of destroyed entities
	void RemovedDestroyedEntities()
	{
		std::vector<Entity*>* temp = new std::vector<Entity*>();
		
		for (auto ent : *scene)
		{
			if (!ent->toBeDestroyed)
				temp->push_back(ent);
		}
		scene->clear();

		for (auto ent : *temp)
		{
			scene->push_back(ent);
		}

		delete temp;
	}

	//restarts the scene
	void Restart()
	{
		LoadLevel(currentLevel);
	}

	//exits the core engine loop
	void QuitGame()
	{
		isRunning = false;
	}

	//loads the next level in the resources folder
	void LoadNextLevel()
	{
		//check if level exists
		std::string fName = "resources/L" + std::to_string(currentLevel + 1) + ".bmp";
		std::ifstream f(fName.c_str());
		if (f.good())
		{
			//std::cout << "Loading " << fName << std::endl;
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

		//if it does, let's generate a level from it!
		for (int i = 0; i < currentLevelSurface->w; i++)
		{
			for (int j = 0; j < currentLevelSurface->h; j++)
			{
				//get the pixel color and pass it to the cached ones
				Uint8 red, green, blue;
				SDL_GetRGB(getpixel(currentLevelSurface, i, j), currentLevelSurface->format, &red, &green, &blue);

				//look in the database for a matching entity and add it to the SceneGraph
				for (Entity* ent : *entityesDB)
				{
					if (ent->color.r == red && ent->color.g == green && ent->color.b == blue)
					{
						AddEntity(ent->name, i * ENTITYSIZE, j * ENTITYSIZE, red, green, blue, ent->spriteIndex);
						//std::cout << "Added entity " << ent->name << std::endl;
					}
				}
			}
		}

		//call Start
		startMethod(this);
	}

	//loads a specific level
	void LoadLevel(int index)
	{
		//check if level exists
		std::string fName = "resources/L" + std::to_string(index) + ".bmp";
		std::ifstream f(fName.c_str());
		if (f.good())
		{
			//std::cout << "Loading " << fName << std::endl;
			currentLevelSurface = IMG_Load(fName.c_str());
		}
		else //if it doesn't exist, we completed all the levels
		{
			Log("Could not load specified level!");
			isRunning = false;
			return;
		}
		//we gucci, clear scene
		scene->clear();

		//if it does, let's generate a level from it!
		for (int i = 0; i < currentLevelSurface->w; i++)
		{
			for (int j = 0; j < currentLevelSurface->h; j++)
			{
				//get the pixel color and pass it to the cached ones
				Uint8 red, green, blue;
				SDL_GetRGB(getpixel(currentLevelSurface, i, j), currentLevelSurface->format, &red, &green, &blue);

				//look in the database for a matching entity and add it to the SceneGraph
				for (Entity* ent : *entityesDB)
				{
					if (ent->color.r == red && ent->color.g == green && ent->color.b == blue)
					{
						AddEntity(ent->name, i * ENTITYSIZE, j * ENTITYSIZE, red, green, blue, ent->spriteIndex);
						//std::cout << "Added entity " << ent->name << std::endl;
					}
				}
			}
		}

		//call Start
		startMethod(this);
	}

	//returns the currently loaded level number
	int GetCurrentLevel()
	{
		return currentLevel;
	}

	//plays the given sound
	void PlaySDLSound(int index)
	{
		if (Mix_PlayChannel(-1, audioClips->at(index), 0) == -1)
			Log("Could not play sound :(");
	}

	//logs a text message to the screen
	void Log(std::string msg)
	{
		std::cout << msg << std::endl;
	}

	//clears the console
	void Clear()
	{
		system("CLS");
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


	//RESOURCES CACHING------------------------------------------------------------------------------------------------------------------------------------------------------------
	
	//loads the background
	bool LoadBackGround()
	{
		background = IMG_Load("resources/background.png");

		if (!background)
			return false;
		else
			return true;
	}

	//loads a font, NOTE: fonts are not working at the moment
	bool LoadFont()
	{
		font = TTF_OpenFont("resources/slkscr.ttf", 12);

		if (!font)
			return false;
		else
			return true;
	}

	//loads all the sprites present in the resources folder, they have to be in the bmp format
	void CacheSprites()
	{
		int currentSprite = 0;
		bool finished = false;
		bool found = false;
		while (!finished)
		{
			//check if sprite exists
			std::string fName = "resources/S" + std::to_string(currentSprite) + ".bmp";
			std::ifstream f(fName.c_str());
			if (f.good())
			{
				//std::cout << "Loading " << fName << std::endl;

				//load image in memory
				SDL_Surface *img = IMG_Load(fName.c_str());
				//set 255,0,255 as our transparent color
				SDL_SetColorKey(img, SDL_TRUE, SDL_MapRGB(screenSurface->format, 255, 0, 255));
				//add to the database
				sprites->push_back(img);
				currentSprite++;
				found = true;
			}
			else //if it doesn't exist, we completed all the levels
			{
				if (!found)
					Log("Could not find sprites in the resources folder");
				finished = true;
			}
		}
		std::cout << "Finished loading sprites, found: " << sprites->size() << std::endl;
	}

	//loads all the audio clips present in the resources folder, they have to be in the wav format
	void CacheAudioClips()
	{
		int currentClip = 0;
		bool finished = false;
		bool found = false;
		while (!finished)
		{
			//check if sound exists
			std::string fName = "resources/A" + std::to_string(currentClip) + ".wav";
			std::ifstream f(fName.c_str());
			if (f.good())
			{
				//std::cout << "Loading " << fName << std::endl;

				//load clip in memory
				Mix_Chunk *chunk = Mix_LoadWAV(fName.c_str());
				//add to the database
				audioClips->push_back(chunk);
				currentClip++;
				found = true;
			}
			else //if it doesn't exist, we completed all the levels
			{
				if (!found)
					Log("Could not find audio clips in the resources folder");
				finished = true;
			}
		}
		std::cout << "Finished loading clips, found: " << audioClips->size() << std::endl;
	}

	//fills a vector of entities initializing them with the specified color, sprite and name, reading them from the Entities.txt file
	void PopulateEntityDatabase()
	{
		//init db
		entityesDB = new std::vector<Entity*>();
		//read Entities file
		std::string line;
		std::ifstream myfile("resources/Entities.txt");
		if (myfile.is_open())
		{
			while (getline(myfile, line))
			{
				//populate database
				std::vector<std::string> entityDescriptor = split(line.c_str(), ' ');
				//an entity is described in the file as: R G B NAME, we build the database from that format
				entityesDB->push_back(new Entity(entityDescriptor[3], //Name
					SDL_Color({ (Uint8)stoi(entityDescriptor[0]), (Uint8)stoi(entityDescriptor[1]), (Uint8)stoi(entityDescriptor[2]) }) //color
					, stoi(entityDescriptor[4]))); //sprite index
			}
			std::cout << "Entities database filled with " << entityesDB->size() << " Entities" << std::endl;
			myfile.close();
		}
		else std::cout << "Unable to open Entities database file";
	}

	//loads all the initial resources
	void AcquireResources()
	{
		if (!LoadBackGround())
		{
			std::cout << "Could not load Background :(" << std::endl;
		}

		/*if (!LoadFont())
		{
			std::cout << "Could not load Font :(" << std::endl;
			isRunning = false;
		}*/

		PopulateEntityDatabase();

		if (!liteMode)
			CacheSprites();

		CacheAudioClips();
		
		Restart();
	}


	//SDL MANAGEMENT---------------------------------------------------------------------------------------------------------------------------------------------------------------

	//creates the SDL window, initializes audio and the various data vectors
	bool Init(std::string name)
	{
		//Initialize SDL
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
		{
			std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
			return false;
		}
		else
		{
			//Create window
			window = SDL_CreateWindow(name.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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

				//init audio
				if (SDL_Init(SDL_INIT_AUDIO) < 0)
				{
					Log("Error initialising SDL mixer");
					return false;
				}
				else
				{
					if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1)
					{
						Log("Error creating audio channels");
						return false;
					}
				}

				//init clips vector
				audioClips = new std::vector<Mix_Chunk*>();

				//init scene vector
				scene = new std::vector<Entity*>();

				//init sprites vector
				sprites = new std::vector<SDL_Surface*>();

				return true;
			}
		}
	}

	//frees all the cached resources and deletes the vectors in memory
	void Terminate()
	{
		//Destroy window
		SDL_DestroyWindow(window);

		//free surfaces
		SDL_FreeSurface(background);

		for (SDL_Surface *surf : *sprites)
		{
			SDL_FreeSurface(surf);
		}

		//free font
		if (font)
			TTF_CloseFont(font);
		font = 0;

		//free audio
		for (Mix_Chunk* chunk : *audioClips)
		{
			Mix_FreeChunk(chunk);
		}

		//Quit SDL subsystems
		SDL_Quit();
		Mix_CloseAudio();

		//clear pointers
		delete scene;
		delete entityesDB;
		delete sprites;

		std::cout << "Game terminated" << std::endl;
		system("PAUSE");
	}


	//MAIN ENGINE METHODS

	//renders the background
	void RenderBackground()
	{
		//draw background
		if (background != NULL)
			SDL_BlitSurface(background, 0, screenSurface, 0);
		else
			SDL_FillRect(screenSurface, NULL, 0);
	}

	//renders all the entities on screen after rendering the background
	void Render()
	{
		RenderBackground();

		//sort sprites by draw order
		std::sort(scene->begin(), scene->end(), sortOrder);

		for (Entity* e : *scene)
		{
			if (e->visible) //only draw visible objects
			{
				//if entity has a sprite, we draw it
				if (e->spriteIndex != -1 && !liteMode)
				{
					int squareLength = sprites->at(e->spriteIndex)->h;
					int frames = sprites->at(e->spriteIndex)->w / squareLength;
					int xOffset = squareLength * e->currentFrame;
					SDL_Rect from{ xOffset, 0, squareLength, squareLength };
					SDL_Rect rect{ e->x, e->y, ENTITYSIZE, ENTITYSIZE };
					SDL_BlitScaled(sprites->at(e->spriteIndex), &from, screenSurface, &rect);
					
					if (e->animate) //if animating, update the frame to the next one
					{
						e->animationTime += e->animationSpeed * deltaTime;
						if (e->animationTime > 100)
						{
							e->animationTime = 0;
							e->currentFrame++;
						}
						if (e->currentFrame >= frames)
							e->currentFrame = 0;
					}
				}
				//if the entity has no sprite attached to it or the engine is running the Lite version, render a square
				else
				{
					const SDL_Rect Rect = { e->x, e->y, ENTITYSIZE, ENTITYSIZE }; //our entities will always be 32 by 32
					Uint32 col = SDL_MapRGB(screenSurface->format, e->color.r, e->color.g, e->color.b);
					SDL_FillRect(screenSurface, &Rect, col);
				}
			}
		}

		//update screen
		SDL_UpdateWindowSurface(window);
	}

	//calls the external Update method by using function pointers
	void Update()
	{
		updateMethod(this);
	}

	//handles SDL events and sets the input states in the relative input arrays
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
			if (e.key.keysym.sym == SDLK_a)
			{
				if (!inputHeld[START])
					inputPressed[START] = true;

				inputHeld[START] = true;
			}
			if (e.key.keysym.sym == SDLK_z)
			{
				if (!inputHeld[A])
					inputPressed[A] = true;

				inputHeld[A] = true;
			}
			if (e.key.keysym.sym == SDLK_x)
			{
				if (!inputHeld[B])
					inputPressed[B] = true;

				inputHeld[B] = true;
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
			}
			if (e.key.keysym.sym == SDLK_DOWN)
			{
				inputHeld[DOWN] = false;
			}
			if (e.key.keysym.sym == SDLK_LEFT)
			{
				inputHeld[LEFT] = false;
			}
			if (e.key.keysym.sym == SDLK_a)
			{
				inputHeld[START] = false;
			}
			if (e.key.keysym.sym == SDLK_z)
			{
				inputHeld[A] = false;
			}
			if (e.key.keysym.sym == SDLK_x)
			{
				inputHeld[B] = false;
			}
		}
	}

	//releases inputs, this is useful for determining if a key is pressed or held
	void ReleaseInputs()
	{
		inputPressed[UP] = false;
		inputPressed[RIGHT] = false;
		inputPressed[DOWN] = false;
		inputPressed[LEFT] = false;
		inputPressed[START] = false;
		inputPressed[A] = false;
		inputPressed[B] = false;

	}


	//UTILITIES--------------------------------------------------------------------------------------------------------------------------------------------------------------------

	//SDL surface pixel access from: http://sdl.beuc.net/sdl.wiki/Pixel_Access
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

	//string tokenizer from: https://stackoverflow.com/questions/53849/how-do-i-tokenize-a-string-in-c
	std::vector<std::string> split(const char *str, char c = ' ')
	{
		std::vector<std::string> result;

		do
		{
			const char *begin = str;

			while (*str != c && *str)
				str++;

			result.push_back(std::string(begin, str));
		} while (0 != *str++);

		return result;
	}
};
