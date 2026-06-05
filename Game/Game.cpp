
#include <slib.h>
using namespace sl;

//good enough
int main()
{
	if (!Init("Game", 640, 480, 0))
	{
		return 1;
	}
	SetTicks(100);
	
	const char* originalPath = "GameLogic.dll";
	const char* copyPath = "GameLogicCopy.dll";
	
	if (!CpyFile(originalPath, copyPath))
	{
		return 1;
	}
	DynamicLibrary lib = OpenDll(copyPath);
	if (!lib)
	{
		return 1;
	}

	GameLogic(*GetGameLogic)() = (GameLogic(*)())GetFunction(lib, "GetGameLogic");
	if (!GetGameLogic)
	{
		return 1;
	}
	GameLogic logic = GetGameLogic();
	logic.Set(GetApi());

	while (!ShouldClose())
	{
		if (false)//here setup the condition that makes the dll reload, could be a keypress
									//or some other fancy stuff you want to make, beware of the loading times
		{
			CloseDll(lib);
			if (!CpyFile(originalPath, copyPath))
			{
				return 1;
			}
			DynamicLibrary lib = OpenDll(copyPath);
			if (!lib)
			{
				return 1;
			}

			GetGameLogic = (GameLogic(*)())GetFunction(lib, "GetGameLogic");
			if (!GetGameLogic)
			{
				return 1;
			}
			logic = GetGameLogic();
			logic.Set(GetApi());
		}

		while (Ticking())
		{
			if (logic.FixedUpdate)
			{
				logic.FixedUpdate(nullptr, nullptr);
			}
		}
		if (logic.Update)
		{
			logic.Update(nullptr, nullptr);
		}
		ClearBuffer();
		if (logic.Render)
		{
			logic.Render(nullptr, nullptr);
		}
		FlipBuffer();
	}
}