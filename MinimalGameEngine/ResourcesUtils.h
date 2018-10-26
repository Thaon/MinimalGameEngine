#pragma once

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

void DrawFont(char* msg)
{
	SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, msg, { 255, 255, 255 }); //color white
	SDL_BlitSurface(surfaceMessage, 0, screenSurface, 0);
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
}