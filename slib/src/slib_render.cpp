#include <iostream>

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include "../include/slib_render.h"

namespace sl
{
	//Forward declarations
	struct API;	
	extern API* Api;

	void PrintError(const char* Header);
	void* GetRenderer();
	void* GetDevice();
	//*Forward declarations

	bool RectCollision(Rect First, Rect Second)
	{
		SDL_FRect first = 
		{ 
			.x = First.x, .y = First.y, 
			.w = First.w, .h = First.h 
		};
		SDL_FRect second =
		{
			.x = Second.x, .y = Second.y,
			.w = Second.w, .h = Second.h
		};

		return SDL_HasRectIntersectionFloat(&first, &second);
	}

	float Dot(Vec2 First, Vec2 Second)
	{
		return First.x * Second.x + First.y * Second.y;
	}
	Vec2 Cross(Vec2 First, Vec2 Second)
	{
		//TODO: implement and understand cross product
		//and its uses
		return { 0, 0 };
	}

	Vec2 operator*(Vec2 Vec, float Scalar)
	{
		return { Vec.x * Scalar, Vec.y * Scalar };
	}
	Vec2 operator*(float Scalar, Vec2 Vec)
	{
		return Vec * Scalar;
	}

	Surface LoadSurface(const char* Path)
	{
		SDL_Surface* Temp = IMG_Load(Path);
		bool Loaded = false;
		if (!Temp)
		{
			PrintError("Failed to load image: ");
			Temp = SDL_CreateSurface(100, 100, SDL_PIXELFORMAT_RGBA8888);
			if (!Temp)
			{
				PrintError("Failed to create surface: ");
				return { nullptr, false };
			}
			Loaded = false;

			//fill with checkerboard pattern
			//adapts depending on size
			for (int i = 0; i < Temp->w; i++)
			{
				for (int j = 0; j < Temp->h; j++)
				{
					if ((i + j) % 2 == 0)
					{
						((uint32_t*)Temp->pixels)[i * Temp->h + j] = 0xfc00ffff;
					}
					else
					{
						((uint32_t*)Temp->pixels)[i * Temp->h + j] = 0x000000ff;
					}
				}
			}
		}
		else
		{
			if (Temp->format != SDL_PIXELFORMAT_RGBA8888)
			{
				SDL_Surface* Temp2 = SDL_ConvertSurface(Temp, SDL_PIXELFORMAT_RGBA8888);
				if (Temp2)
				{
					SDL_DestroySurface(Temp);
					Temp = Temp2;
					SDL_DestroySurface(Temp2);
				}
				else
				{
					PrintError("Failed to convert texture format: ");
				}
			}
			Loaded = true;
		}
		
		return { Temp, Loaded };
	}

	Surface::~Surface()
	{
		if(Data)
		{
			DestroySurface(*this);
		}
	}
	void DestroySurface(Surface& surface)
	{
		SDL_DestroySurface((SDL_Surface*)surface.Data);
		surface.Data = nullptr;
	}

	//bullshit i made to change what function is ran
	static bool RenderNormal(void* Renderer, void* Data, void* Source, void* Destination)
	{
		return SDL_RenderTexture((SDL_Renderer*)Renderer, (SDL_Texture*)Data, (SDL_FRect*)Source, (SDL_FRect*)Destination);
	}
	static bool RenderTiled(void*  Renderer, void *Data, void* Source, void* Destination)
	{
		float Scale = 50;

		return SDL_RenderTextureTiled((SDL_Renderer*)Renderer, (SDL_Texture*)Data, nullptr, Scale, (SDL_FRect*)Destination);
	}
	//*bullshit i made to change what function is ran
	
	Texture LoadTexture(const char* Path)
	{
		Surface Temp = LoadSurface(Path);

		if (!Temp.Data)
		{
			return { nullptr, nullptr };
		}

		Temp.Data = SDL_CreateTextureFromSurface((SDL_Renderer*)GetRenderer(), (SDL_Surface*)Temp.Data);
		if (!Temp.Data)
		{
			return { nullptr, nullptr };
		}
		if (!SDL_SetTextureScaleMode((SDL_Texture*)Temp.Data, SDL_SCALEMODE_PIXELART))
		{
			PrintError("Failed to set texture scale mode: ");
			//don't return, Texture is technically valid
		}

		//check wether to tile the texture(aka it's a checker board)
		bool (*RenderFunction)(void*, void*, void*, void*) = nullptr;
		if (!Temp.Loaded)
		{
			RenderFunction = RenderTiled;
		}
		else
		{
			RenderFunction = RenderNormal;
		}

		return { Temp.Data, RenderFunction };
	}

	static SDL_FRect ToSDLFRect(Rect rect)
	{
		return { rect.x, rect.y, rect.w, rect.h };
	}
	void RenderTexture(Texture &texture)
	{
		if (!texture.RenderFunction(SDL_GetRendererFromTexture((SDL_Texture*)texture.Data), texture.Data, nullptr, nullptr))
		{
			PrintError("Failed to render texture: ");
		}
	}
	void RenderTexture(Texture &texture, Rect Destination)
	{
		SDL_FRect dst = ToSDLFRect(Destination);

		if (!texture.RenderFunction(SDL_GetRendererFromTexture((SDL_Texture*)texture.Data), texture.Data, nullptr, &dst))
		{
			PrintError("Failed to render texture: ");
		}
	}
	void RenderTexture(Texture &texture, Rect Source, Rect Destination)
	{
		SDL_FRect dst = ToSDLFRect(Destination);
		SDL_FRect src = ToSDLFRect(Source);

		if (!texture.RenderFunction(SDL_GetRendererFromTexture((SDL_Texture*)texture.Data), texture.Data, &src, &dst))
		{
			PrintError("Failed to render texture: ");
		}
	}
	void DestroyTexture(Texture &texture)
	{
		SDL_DestroyTexture((SDL_Texture*)texture.Data);
		texture.Data = nullptr;
	}
	Texture::~Texture()
	{
		if(Data)
		{
			DestroyTexture(*this);
		}
	}

	Shader LoadShader(const char* Path)
	{
		size_t size = 0;
		void *file = SDL_LoadFile(Path, &size);
		if (!file)
		{
			PrintError("Failed to load  shader file: ");
			return { nullptr };
		}
		SDL_GPUShaderCreateInfo info = { 0 };
		info.code = (uint8_t*)file;
		info.code_size = size;
		info.entrypoint = "main";
		info.format = SDL_GPU_SHADERFORMAT_SPIRV;
		info.stage = SDL_GPU_SHADERSTAGE_FRAGMENT;

		SDL_GPUShader* shader = SDL_CreateGPUShader((SDL_GPUDevice*)GetDevice(), &info);
		if (!shader)
		{
			PrintError("Failed to create shader: ");
			return { nullptr };
		}

		SDL_GPURenderStateCreateInfo stateInfo =
		{
			.fragment_shader = shader
		};
		SDL_GPURenderState* state = SDL_CreateGPURenderState((SDL_Renderer*)GetRenderer(), &stateInfo);
		if (!state)
		{
			PrintError("Failed to create render state");
			return { nullptr };
		}

		return { state };
	}
	void BeginShader(Shader& shader)
	{
		if (!SDL_SetGPURenderState((SDL_Renderer*)GetRenderer(), (SDL_GPURenderState*)shader.Data))
		{
			PrintError("Failed to set gpu render state: ");
		}
	}
	void EndShader()
	{
		if (!SDL_SetGPURenderState((SDL_Renderer*)GetRenderer(), nullptr))
		{
			PrintError("Failed to unset gpu render state: ");
		}
	}

	void RenderPoint(Point point, Color color)
	{
		uint8_t r, g, b, a;
		SDL_GetRenderDrawColor((SDL_Renderer*)GetRenderer(), &r, &g, &b, &a);
		SDL_SetRenderDrawColor((SDL_Renderer*)GetRenderer(), color.r, color.g, color.b, color.a);

		if (!SDL_RenderPoint((SDL_Renderer*)GetRenderer(), point.x, point.y))
		{
			PrintError("Failed to render point: ");
		}

		SDL_SetRenderDrawColor((SDL_Renderer*)GetRenderer(), r, g, b, a);
	}
	void RenderLine(Point StartPoint, Point EndPoint, Color color)
	{
		uint8_t r, g, b, a;
		SDL_GetRenderDrawColor((SDL_Renderer*)GetRenderer(), &r, &g, &b, &a);
		SDL_SetRenderDrawColor((SDL_Renderer*)GetRenderer(), color.r, color.g, color.b, color.a);

		if (!SDL_RenderLine((SDL_Renderer*)GetRenderer(), StartPoint.x, StartPoint.y, EndPoint.x, EndPoint.y))
		{
			PrintError("Failed to render line: ");
		}

		SDL_SetRenderDrawColor((SDL_Renderer*)GetRenderer(), r, g, b, a);
	}
}