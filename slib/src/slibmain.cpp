#include <iostream>

#include <SDL3/SDL.h>

#include "../include/slib.h"

using namespace sl;

int main()
{
	if (!Init("static lib", 600, 600, 0x20))
	{
		return 1;
	}

	Texture texture = LoadTexture("image.jpg");

	Sound sound = LoadSound("music.mp3");

	while (!sl::ShouldClose())
	{
		while (sl::Ticking())
		{
			//fixed update goes here(does matter)

			//
		}
			
		//event handling goes here (doesnt't matter really, just don't put in fixed update)

		//
		sl::ClearBuffer();
		//rendering goes here (does matter)
	
		
		sl::FlipBuffer();

		sl::Delay(16);
	}
	
	sl::Quit();
}
