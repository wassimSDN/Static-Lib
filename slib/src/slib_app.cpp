#include <iostream>

#if defined(_WIN32)
#include <Windows.h>
#endif

#include<SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>

#include "../include/slib_app.h"

namespace sl
{
	struct API
	{
		SDL_Window* Window = nullptr;
		SDL_Renderer* Renderer = nullptr;
		SDL_GPUDevice* GPU = nullptr;
		MIX_Mixer* Mixer = nullptr;

		bool CurrentKeyStates[SDL_SCANCODE_COUNT]	= { false };
		bool PreviousKeyStates[SDL_SCANCODE_COUNT]	= { false };

		bool CurrentMouseStates[(int)Mouse::count]	= { false };
		bool PreviousMouseStates[(int)Mouse::count] = { false };
		bool Motion = false;
		float x = 0.f, y = 0.f;

		bool Running = true;

		long double DeltaTime = 0.;
		long double LastTime = 0.;
		long double TimeStep = 0.;
		long double AccumulatedTime = 0.;
	};
	API* Api = nullptr;

	void SetApi(void* ApiP)
	{
		if (!ApiP)
		{
			std::cerr << "Api is NULL\n";
			return;
		}

		Api = (API*)ApiP;
	}
	void* GetApi()
	{
		return Api;
	}

	void* GetRenderer()
	{
		return Api->Renderer;
	}
	void* GetWindow()
	{
		return Api->Window;
	}
	void *GetDevice()
	{
		return Api->GPU;
	}
	void* GetMixer()
	{
		return Api->Mixer;
	}

	void PrintError(const char *Header)
	{
		std::cerr << Header << SDL_GetError() << '\n';
	}

	void PrintWindowsError()
	{
		LPSTR Message = nullptr;
		FormatMessageA(	FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
						nullptr, GetLastError(), 0, (LPSTR)&Message, 0, nullptr);
		
		std::cout << Message << '\n';

		LocalFree(Message);
	}

	bool Init(const char* Title, int Width, int Height, int Flags)
	{
		try
		{
			Api = new API;
		}
		catch (std::bad_alloc& e)
		{
			std::cerr << "Failed to Allocate for API: " << e.what() << '\n';
			return false;
		}
		std::cout << "Api allocated\n";

		if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD | SDL_INIT_CAMERA))
		{
			PrintError("Failed to initialize: ");
			return false;
		}
		std::cout << "SDL Initialized\n";

		Api->Window = SDL_CreateWindow(Title, Width, Height, Flags);
		if (!Api->Window)
		{
			PrintError("Failed to create Window: ");
			return false;
		}
		std::cout << "Window Created\n";
		
		const char Driver[] = "vulkan"; //using vulkan because fuck this entire thing 
		
		Api->GPU = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, Driver);
		if (!Api->GPU)
		{
			PrintError("Failed to Create GPU device: ");
			return false;
		}
		std::cout << "Created GPU device\n";

		Api->Renderer = SDL_CreateGPURenderer(Api->GPU, Api->Window);
		if (!Api->Renderer)
		{
			PrintError("Failed to create renderer: ");
			return false;
		}
		std::cout << "GPU Renderer Created With " << Driver << '\n';

		if (!MIX_Init())
		{
			PrintError("Failed to init Mixer: ");
			return false;
		}
		std::cout << "Initialized Mixer\n";
		
		Api->Mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
		if (!Api->Mixer)
		{
			PrintError("Failed to create Mixer device: ");
			return false;
		}
		std::cout << "Created mixer device\n";

		Api->LastTime = (long double)SDL_GetPerformanceCounter() / (long double)SDL_GetPerformanceFrequency();
		SetTicks(-1);
		return true;
	}
	void Quit()
	{
		SDL_DestroyRenderer(Api->Renderer);
		std::cout << "Renderer Destroyed\n";
		
		SDL_DestroyWindow(Api->Window);
		std::cout << "Window Destroyed\n";
		
		SDL_Quit();
		std::cout << "SDL Cleaned Up\n";

		delete Api;
		std::cout << "Api Freed\n";
	}
	int WindowX()
	{
		int x = 0;
		if (!SDL_GetWindowPosition(Api->Window, &x, nullptr))
		{
			PrintError("Failed to get window X position: ");
		}

		return x;
	}
	int WindowY()
	{
		int y = 0;
		if (!SDL_GetWindowPosition(Api->Window, nullptr, &y))
		{
			PrintError("Failed to get window Y position: ");
		}

		return y;
	}
	int WindowWidth()
	{
		int w = 0;
		if (!SDL_GetWindowSize(Api->Window, &w, nullptr))
		{
			PrintError("Failed to get window width: ");
		}

		return w;
	}
	int WindowHeight()
	{
		int h = 0;
		if (!SDL_GetWindowSize(Api->Window, nullptr, &h))
		{
			PrintError("Failed to get window height: ");
		}

		return h;
	}

	void ClearBuffer()
	{
		SDL_RenderClear(Api->Renderer);
	}
	void FlipBuffer()
	{
		SDL_RenderPresent(Api->Renderer);
	}
	void Delay(uint64_t Delayms)
	{
		SDL_DelayPrecise(Delayms * 1000000);
	}

	void SetTicks(int Ticks)
	{
		if (Ticks > 0)
		{
			Api->TimeStep = 1.0 / Ticks;
			return;
		}

		Api->TimeStep = 1.0 / 50;
	}

	bool Ticking()
	{
		if (Api->AccumulatedTime < Api->TimeStep)
			return false;

		Api->AccumulatedTime -= Api->TimeStep;

		return true;
	}

	void Update()
	{
		long double CurrentTime = (long double)SDL_GetPerformanceCounter() / (long double)SDL_GetPerformanceFrequency();
		Api->DeltaTime = CurrentTime - Api->LastTime;
		Api->LastTime = CurrentTime;
		Api->AccumulatedTime += Api->DeltaTime;

		memcpy(Api->PreviousKeyStates, Api->CurrentKeyStates, (size_t)Key::count * sizeof(bool));
		memcpy(Api->PreviousMouseStates, Api->CurrentMouseStates, (size_t)Mouse::count * sizeof(bool));

		SDL_Event e = { 0 };
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			//termination requested
			case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
			case SDL_EVENT_QUIT:
				Api->Running = false;
				break;

			//keyboard events
			case SDL_EVENT_KEY_DOWN:
				Api->CurrentKeyStates[e.key.scancode] = true;
				break;
			case SDL_EVENT_KEY_UP:
				Api->CurrentKeyStates[e.key.scancode] = false;
				break;

			//mouse events
			case SDL_EVENT_MOUSE_BUTTON_DOWN:
				Api->CurrentMouseStates[e.button.button] = true;
				break;
			case SDL_EVENT_MOUSE_BUTTON_UP:
				Api->CurrentMouseStates[e.button.button] = false;
				break;
			case SDL_EVENT_MOUSE_MOTION:
				Api->Motion = true;
				Api->x = e.motion.x;
				Api->y = e.motion.y;
				break;
			}
		}
	}

	bool ShouldClose()
	{
		Update();
		return !Api->Running;
	}


	DynamicLibrary OpenDll(const char* Path)
	{
#if defined(_WIN32)
		HMODULE Library = LoadLibraryA(Path);
		if (!Library)
		{
			std::cerr << "Failed to load dll(" << Path << "): ";
			PrintWindowsError();
			
			return nullptr;
		}

		return Library;
#elif defined(__linux__)
		return nullptr;
#else
		return nullptr
#endif
	}
	void CloseDll(DynamicLibrary DLL)
	{
#if defined(_WIN32)
		FreeLibrary((HMODULE)DLL);
#elif defined(__linux__)
#else
#endif
	}
	void* GetFunction(DynamicLibrary DLL, const char* FunctionSignature)
	{
#if defined(_WIN32)
		void* FunctionAddress = GetProcAddress((HMODULE)DLL, FunctionSignature);
		if (!FunctionAddress)
		{
			std::cerr << "Failed to load function (" << FunctionSignature << "): ";
			PrintWindowsError();

			return nullptr;
		}
		
		return FunctionAddress;
#elif defined(__linux__) 
		//TODO, implement this bs for linux (screw other os's)
		return nullptr;
#else
		return nullptr;
#endif
	}
	
	//workaroung the '#define CopyFile' in 'Windows.h'
	bool CpyFile(const char* Source, const char* Destination)
	{
#if defined(WIN32)
		if (!CopyFileA(Source, Destination, FALSE))
		{
			std::cerr << "Failed to copy file '" << Source << "' to '" << Destination << "': ";
			PrintWindowsError();
			return false;
		}

		return true;

#elif defined(__linux__)
		return false;
#else
		return false;
#endif
	}

	float MouseX()
	{
		return Api->x;
	}
	float MouseY()
	{
		return Api->y;
	}
	bool MouseDown(Mouse mouse)
	{
		return Api->CurrentMouseStates[(int)mouse];
	}
	bool MouseJustDown(Mouse mouse)
	{
		return MouseDown(mouse) && !Api->PreviousMouseStates[(int)mouse];
	}
	bool MouseUp(Mouse mouse)
	{
		return !Api->CurrentMouseStates[(int)mouse];
	}
	bool MouseJustUp(Mouse mouse)
	{
		return MouseUp(mouse) && Api->PreviousMouseStates[(int)mouse];
	}

	bool KeyDown(Key key)
	{
		return Api->CurrentKeyStates[(int)key];
	}
	bool KeyJustDown(Key key)
	{
		return KeyDown(key) && !Api->PreviousKeyStates[(int)key];
	}
	bool KeyUp(Key key)
	{
		return !Api->CurrentKeyStates[(int)key];
	}
	bool KeyJustUp(Key key)
	{
		return KeyUp(key) && Api->PreviousKeyStates[(int)key];
	}
}