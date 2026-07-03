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
		bool CurrentKeyStates[SDL_SCANCODE_COUNT] = { false };
		bool PreviousKeyStates[SDL_SCANCODE_COUNT] = { false };

		bool CurrentMouseStates[(int)Mouse::count] = { false };
		bool PreviousMouseStates[(int)Mouse::count] = { false };

		SDL_Window* Window = nullptr;
		SDL_Renderer* Renderer = nullptr;
		SDL_GPUDevice* Device = nullptr;
		MIX_Mixer* Mixer = nullptr;

		long double DeltaTime = 0.;
		long double LastTime = 0.;
		long double TimeStep = 0.;
		long double AccumulatedTime = 0.;

		float x = 0.f, y = 0.f;

		bool Motion = false;
		
		bool Running = true;
	};

	//********Globals********//
	API* Api = nullptr;
	//*****End Of Globals**//

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
		return Api->Device;
	}
	void* GetMixer()
	{
		return Api->Mixer;
	}

	//crappy "logging" ""system""
	void PrintSDLError(const char *Message)
	{
		std::cerr << "[ERROR] " << Message << SDL_GetError() << '\n';
	}
	void PrintError(const char* Message)
	{
		std::cerr << "[ERROR] " << Message << '\n';
	}
	void PrintInfo(const char* Message)
	{
		std::cout << "[INFO] " << Message << '\n';
	}
	void PrintWarning(const char* Message)
	{
		std::cerr << "[WARNING] " << Message << '\n';
	}

	void PrintWindowsError()
	{
		LPSTR Message = nullptr;
		FormatMessageA(	FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
						nullptr, GetLastError(), 0, (LPSTR)&Message, 0, nullptr);
		
		std::cerr << Message << '\n';

		LocalFree(Message);
	}

	bool Init(const char* Title, int Width, int Height, int Flags)
	{
		int major = SDL_VERSIONNUM_MAJOR(SDL_GetVersion());
		int minor = SDL_VERSIONNUM_MINOR(SDL_GetVersion());
		int micro = SDL_VERSIONNUM_MICRO(SDL_GetVersion());
		std::cout << "[INFO] Using SDL " << major << '.' << minor << '.' << micro << '\n';

		try
		{
			Api = new API;
		}
		catch (std::bad_alloc& e)
		{
			std::cerr << "Failed to Allocate for API: " << e.what() << '\n';
			return false;
		}
		PrintInfo("Api allocated");

		if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD | SDL_INIT_CAMERA ))
		{
			PrintSDLError("Failed to initialize: ");
			return false;
		}
		PrintInfo("SDL Initialized");

		Api->Window = SDL_CreateWindow(Title, Width, Height, Flags);
		if (!Api->Window)
		{
			PrintSDLError("Failed to create Window: ");
			return false;
		}
	
		char Driver[] = "vulkan"; //using vulkan because fuck this entire thing 
		Api->Device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, Driver);
		if (!Api->Device)
		{
			PrintSDLError("Failed to get renderer gpu device: ");
			return false;
		}

		Api->Renderer = SDL_CreateGPURenderer(Api->Device, Api->Window);
		if (!Api->Renderer)
		{
			PrintSDLError("Failed to create renderer: ");
			return false;
		}
		
		if (!MIX_Init())
		{
			PrintSDLError("Failed to init Mixer: ");
			return false;
		}
		PrintInfo("Initialized Mixer");

		Api->Mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
		if (!Api->Mixer)
		{
			PrintSDLError("Failed to create Mixer device: ");
			return false;
		}
		PrintInfo("Created mixer device");

		Api->LastTime = (long double)SDL_GetPerformanceCounter() / (long double)SDL_GetPerformanceFrequency();
		SetTicks(-1);

		return true;
	}
	void Quit()
	{
		MIX_DestroyMixer(Api->Mixer);
		PrintInfo("Mixer Destroyed");

		SDL_DestroyRenderer(Api->Renderer);
		PrintInfo("Renderer Destroyed");

		SDL_DestroyGPUDevice(Api->Device);
		PrintInfo("GPU Device Destroyed");

		SDL_DestroyWindow(Api->Window);
		PrintInfo("Window Destroyed");

		MIX_Quit();
		PrintInfo("Mixer Clean Up");

		SDL_Quit();
		PrintInfo("SDL Cleaned Up");

		delete Api;
		PrintInfo("Api Freed");
	}
	int WindowX()
	{
		int x = -1;
		if (!SDL_GetWindowPosition(Api->Window, &x, nullptr))
		{
			PrintSDLError("Failed to get window X position: ");
		}

		return x;
	}
	int WindowY()
	{
		int y = -1;
		if (!SDL_GetWindowPosition(Api->Window, nullptr, &y))
		{
			PrintSDLError("Failed to get window Y position: ");
		}

		return y;
	}
	int WindowWidth()
	{
		int w = 0;
		if (!SDL_GetWindowSize(Api->Window, &w, nullptr))
		{
			PrintSDLError("Failed to get window width: ");
		}

		return w;
	}
	int WindowHeight()
	{
		int h = 0;
		if (!SDL_GetWindowSize(Api->Window, nullptr, &h))
		{
			PrintSDLError("Failed to get window height: ");
		}

		return h;
	}

	void ClearBuffer(int r, int g, int b, int a)
	{
		if(!SDL_SetRenderDrawColor(Api->Renderer, r, g, b, a))
		{
			PrintSDLError("Failed to set clear color: ");
		}
		if (!SDL_RenderClear(Api->Renderer))
		{
			PrintSDLError("Failed to clear buffer: ");
		}
	}
	void FlipBuffer()
	{
		if (!SDL_RenderPresent(Api->Renderer))
		{
			PrintSDLError("Failed to flip buffer: ");
		}
	}
	long double DeltaTime()
	{
		return Api->DeltaTime;
	}
	float CurrentTime()
	{
		return SDL_GetTicks() / 1000.f;
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