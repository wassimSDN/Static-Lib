#include <iostream>

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include "../include/slib_render.h"

namespace sl
{
	//Forward declarations
	struct API;	
	void PrintSDLError(const char* Message);
	void PrintInfo(const char* Message);
	void PrintWarning(const char* Message);
	void* GetRenderer();
	void* GetDevice();
	void* GetWindow();
	//*Forward declarations

	//********Globals********//
	extern API* Api;
	extern SDL_GPUCommandBuffer* CommandBuffer;
	extern SDL_GPURenderPass* RenderPass;
	extern SDL_GPUTexture* SwapChaintexture;
	extern SDL_GPUGraphicsPipeline* Pipeline;
	extern SDL_GPUShader *VertexShader;
	extern SDL_GPUShader* FragmentShader;
	//*****End Of Globals**//


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
			PrintSDLError("Failed to load image: ");
			Temp = SDL_CreateSurface(100, 100, SDL_PIXELFORMAT_RGBA8888);
			if (!Temp)
			{
				PrintSDLError("Failed to create surface: ");
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
				}
				else
				{
					PrintSDLError("Failed to convert texture format: ");
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

	void CreateGraphicsPipeline()
	{

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
			PrintSDLError("Failed to set texture scale mode: ");
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
			PrintSDLError("Failed to render texture: ");
		}
	}
	void RenderTexture(Texture &texture, Rect Destination)
	{
		SDL_FRect dst = ToSDLFRect(Destination);

		if (!texture.RenderFunction(SDL_GetRendererFromTexture((SDL_Texture*)texture.Data), texture.Data, nullptr, &dst))
		{
			PrintSDLError("Failed to render texture: ");
		}
	}
	void RenderTexture(Texture &texture, Rect Source, Rect Destination)
	{
		SDL_FRect dst = ToSDLFRect(Destination);
		SDL_FRect src = ToSDLFRect(Source);

		if (!texture.RenderFunction(SDL_GetRendererFromTexture((SDL_Texture*)texture.Data), texture.Data, &src, &dst))
		{
			PrintSDLError("Failed to render texture: ");
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

	Shader LoadShader(const char* Path, int NumberOfUniformBuffers)
	{
		void* ResultShader = nullptr;
		void* ResultRenderState = nullptr;

		size_t Size = 0;
		uint8_t* Code = (uint8_t*)SDL_LoadFile(Path, &Size);
		if (!Code)
		{
			PrintSDLError("Failed to load shader file: ");
			return { nullptr, nullptr };
		}

		SDL_GPUShaderCreateInfo ShaderInfo = { 0 };
		ShaderInfo.code = Code;
		ShaderInfo.code_size = Size;
		ShaderInfo.entrypoint = "main"; //the SDL example doesn't mention this for some reason? 
										  //shaders still work though
		ShaderInfo.format = SDL_GPU_SHADERFORMAT_SPIRV;
		ShaderInfo.stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
		ShaderInfo.num_samplers = 1;
		ShaderInfo.num_uniform_buffers = NumberOfUniformBuffers;

		ResultShader = SDL_CreateGPUShader((SDL_GPUDevice*)GetDevice(), &ShaderInfo);
		SDL_free(Code);
		if (!ResultShader)
		{
			PrintSDLError("Failed to create shader: ");
			return { nullptr, nullptr };
		}

		SDL_GPURenderStateCreateInfo StateInfo = { 0 };
		StateInfo.fragment_shader = (SDL_GPUShader*)ResultShader;
		ResultRenderState = SDL_CreateGPURenderState((SDL_Renderer*)GetRenderer(), &StateInfo);
		if (!ResultRenderState)
		{
			SDL_ReleaseGPUShader((SDL_GPUDevice*)GetDevice(), (SDL_GPUShader*)ResultShader);
			PrintSDLError("Failed to create render state: ");
			return { nullptr, nullptr };
		}

		return { ResultShader, ResultRenderState };
	}
	void PushUniforms(Shader &shader, int Slot, void* Data, size_t Size)
	{
		if (!SDL_SetGPURenderStateFragmentUniforms((SDL_GPURenderState*)shader.RenderState, Slot, Data, Size))
		{
			PrintSDLError("Failed to push uniforms: ");
		}
	}
	void BeginShader(Shader& shader)
	{
		if (!SDL_SetGPURenderState((SDL_Renderer*)GetRenderer(), (SDL_GPURenderState*)shader.RenderState))
		{
			PrintSDLError("Failed to set gpu render state: ");
		}
	}
	void EndShader()
	{
		if (!SDL_SetGPURenderState((SDL_Renderer*)GetRenderer(), nullptr))
		{
			PrintSDLError("Failed to set gpu render state: ");
		}
	}
	void DestroyShader(Shader& shader)
	{
		SDL_ReleaseGPUShader((SDL_GPUDevice*)GetDevice(), (SDL_GPUShader*)shader.Shader_);
		SDL_DestroyGPURenderState((SDL_GPURenderState*)shader.RenderState);
	}
	Shader::~Shader()
	{
		DestroyShader(*this);
	}

	void RenderPoint(Point point, Color color)
	{
		uint8_t r, g, b, a;
		SDL_GetRenderDrawColor((SDL_Renderer*)GetRenderer(), &r, &g, &b, &a);
		SDL_SetRenderDrawColor((SDL_Renderer*)GetRenderer(), color.r, color.g, color.b, color.a);

		if (!SDL_RenderPoint((SDL_Renderer*)GetRenderer(), point.x, point.y))
		{
			PrintSDLError("Failed to render point: ");
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
			PrintSDLError("Failed to render line: ");
		}

		SDL_SetRenderDrawColor((SDL_Renderer*)GetRenderer(), r, g, b, a);
	}
}