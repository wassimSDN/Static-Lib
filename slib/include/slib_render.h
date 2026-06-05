#pragma once

#include <cstdint>

namespace sl
{
	struct Point
	{
		float x = 0, y = 0;
	};
	
	struct Color
	{
		uint8_t r, g, b, a;
	};

	struct Surface;
	Surface LoadSurface(const char* Path);

	struct Surface
	{
		void* Data = nullptr; //treated as SDL_Surface* internally
		bool Loaded = false;

		~Surface();
	};
	void DestroySurface(Surface& surface);

	struct Rect
	{
		float x = 0, y = 0;
		float w = 0, h = 0;
	};
	bool RectCollision(Rect First, Rect Second);

	struct Vec2
	{
		float x = 0;
		float y = 0;
	};

	float Dot(Vec2 First, Vec2 Second);
	Vec2 Cross(Vec2 First, Vec2 Second);

	Vec2 operator*(Vec2 Vec, float Scalar);
	Vec2 operator*(float Scalar, Vec2 Vec);
	
	struct Texture;
	Texture LoadTexture(const char* Path);

	struct Texture
	{
		void* Data = nullptr; //treated as SDL_Texture* internally
		bool (*RenderFunction)(void*, void*, void*, void*) = nullptr; //for internal

		~Texture();
	};

	void RenderTexture(Texture &texture);
	void RenderTexture(Texture &texture, Rect Destination);
	void RenderTexture(Texture &texture, Rect Source, Rect Destination);
	void DestroyTexture(Texture &texture);

	struct Shader;
	Shader LoadShader(const char* Path);

	struct Shader
	{
		void* Data = nullptr;
	};

	void BeginShader(Shader& shader);
	void EndShader();

	void RenderPoint(Point point, Color color = {0, 0, 0, 0});
	void RenderLine(Point StartPoint, Point EndPoint, Color color = {0, 0, 0, 0});
}