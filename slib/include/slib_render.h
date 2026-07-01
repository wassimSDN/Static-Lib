#pragma once

#include <cstdint>

namespace sl
{
	/**
	* a structure that represents a color with RGBA32 format.
	*/
	struct Color
	{
		uint8_t r, g, b, a;
	};

	/**
	* a structure that represents a point.
	*
	* \sa RenderPoint
	* \sa RenderLine
	*/
	struct Point
	{
		float x = 0, y = 0;
	};
	/**
	* Render a single point on the window with a color.
	*
	* \param point the position on the window.
	* \param color the color to use.
	*
	* \sa RenderLine
	*/
	void RenderPoint(Point point, Color color = { 0, 0, 0, 0 });
	/**
	* Render a line on the window.
	*
	* \param StartPoint the start of the line.
	* \param EndPoint the end of the line
	* \param color the color to use.
	*
	* \sa RenderPoint
	*/
	void RenderLine(Point StartPoint, Point EndPoint, Color color = { 0, 0, 0, 0 });
	
	/**
	* a structure that represents a rectangle.
	*
	* The origin of the coordinate space is in the top-left, with increasing
	* values moving down and right. The properties `x` and `y` represent the
	* coordinates of the top-left corner of the rectangle.
	*
	* \sa RectCollision
	*/
	struct Rect
	{
		float x = 0, y = 0;
		float w = 0, h = 0;
	};
	/**
	* Check the collision between two rectangles, i.e they have an intersection.
	*
	* \param First the first rectangle.
	* \param Second the second rectangle
	* \returns true if there is a collision, false otherwise
	*/
	bool RectCollision(Rect First, Rect Second);

	/**
	* a structure that represents a vector.
	*
	* \sa Dot
	* \sa Cross
	*/
	struct Vec2
	{
		float x = 0;
		float y = 0;
	};
	/**
	* Calculate the Dot-Product of two vectors.
	*
	* \param First the first vector.
	* \param Second the second vector.
	* \returns The result of the Dot-Product.
	*/
	float Dot(Vec2 First, Vec2 Second);
	/**
	* Calculate the Cross-Product of two vectors.
	*
	* \param First the first vector.
	* \param Second the second vector.
	* \returns The resulting vector of the Cross-Product.
	*/
	Vec2 Cross(Vec2 First, Vec2 Second);
	/**
	* Scale a vector by a scalar.
	*
	* \param Vec the vector to be scaled.
	* \param Scalar the scaling amount.
	* \returns The scaled vector
	*/
	Vec2 operator*(Vec2 Vec, float Scalar);
	/**
	* Scale a vector by a scalar.
	*
	* \param Scalar the scaling amount.
	* \param Vec the vector to be scaled.
	* \returns The scaled vector
	*/
	Vec2 operator*(float Scalar, Vec2 Vec);

	/**
	* a structure that represents pixel data in RAM.
	*
	* \sa LoadSurface
	* \sa DestroySurface
	*/
	struct Surface
	{
		void* Data = nullptr; /**< treated as SDL_Surface* internally */
		bool Loaded = false;

		~Surface();
	};
	/**
	 * Load an image from disk into a Surface.
	 *
	 * \param Path the path to the image file.
	 * \returns a Surface structure with pixels, makes a CheckerBoard-Pattern
	 *			if the image wasn't found
	 * 
	 * \sa DestroySurface
	 */
	Surface LoadSurface(const char* Path);
	/**
	* Free a surface from RAM.
	*
	* \param surface the surface to free.
	*
	* \sa LoadSurface
	*/
	void DestroySurface(Surface& surface);
	
	/**
	* a structure that represents pixel data in VRAM.
	*
	* \sa LoadTexture
	* \sa DestroyTexture
	* \sa RenderTexture
	*/
	struct Texture
	{
		void* Data = nullptr;                                         /**< Treated as SDL_Texture* internally */
		bool (*RenderFunction)(void*, void*, void*, void*) = nullptr; /**< Used for rendering, you shouldn't touch this*/

		~Texture();
	};
	/**
	* Load an image from disk into a Surface, uses a temporary Surface internally.
	* 
	* \param Path the path to the image file.
	* \returns a texture with regular render function if the image was found,
	*		   a texture with tiled render function otherwise
	*/
	Texture LoadTexture(const char* Path);
	/**
	* Render a texture to fit the entire window.
	*
	* \param texture the texture to render.
	* 
	* \sa DestroyTexture
	*/
	void RenderTexture(Texture &texture);
	/**
	* Render a texture to fit a destination rectangle on the window.
	*
	* \param texture the texture to render.
	* \param Destination where to render the texture on the window.
	*/
	void RenderTexture(Texture &texture, Rect Destination);
	/**
	* Render a portion of a texture to fit a destination rectangle on the window.
	*
	* \param texture the texture to render.
	* \param Source the portion of the texture to render on the window.
	* \param Destination where to render the texture on the window.
	*/
	void RenderTexture(Texture &texture, Rect Source, Rect Destination);
	/**
	* Free a texture from VRAM.
	*
	* \param texture texture to free.
	*
	* \sa LoadTexture
	*/
	void DestroyTexture(Texture &texture);

	//Incomplete API, DO NOT USE
	//TODO refactor slib to complete the shader interface
	struct Shader;
	Shader LoadShader(const char* Path, int NumberOfUniformBuffers);
	void PushUniforms(Shader& shader, int slot, void* Data, size_t size);
	void BeginShader(Shader& shader);
	void EndShader();
	void DestroyShader(Shader& shader);

	struct Shader
	{
		void* Shader_ = nullptr;
		void* RenderState = nullptr;

		~Shader();
	};
	//*Incomplete API, DO NOT USE
}