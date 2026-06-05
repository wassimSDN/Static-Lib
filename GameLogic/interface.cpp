#include "interface.h"

#include <iostream>

GameLogic logic = { 0 };	//here setup your own function depending on what you're doing
							//then when using 'GetGameLogic' just call them in Game.cpp
							//the architecture from here is your own to manage

extern "C"
{
	__declspec(dllexport) GameLogic GetGameLogic()
	{
		return logic;
	}
}
