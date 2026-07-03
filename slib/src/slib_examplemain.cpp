#include <iostream>

#include <SDL3/SDL.h>

#include "../include/slib.h"
using namespace sl;

struct Uniform
{
	float random;
	float intenstiy;
	float time;
	bool invertColor; 
};

int main()
{
	if (!Init("example main", 700, 700, 32))
	{
		return 1;
	}
	SetTicks(100);

	{
		Texture tex = LoadTexture("wrong_path.png");
		Texture tex2 = LoadTexture("sample.png");
		if (!tex.Data || !tex2.Data)
		{
			return 1;
		}

		Shader shader = LoadShader("high_shader.spv", 1);

		Uniform uniform = {};
		uniform.intenstiy = 0;
		uniform.random = 0;

		while (!ShouldClose())
		{
			while (Ticking())
			{

			}

			uniform.invertColor = MouseDown(Mouse::left);

			ClearBuffer();

			PushUniforms(shader, 0, &uniform, sizeof(Uniform));
			BeginShader(shader);
			
			RenderTexture(tex, {200, 200, 200, 200});
			RenderTexture(tex, { MouseX(), MouseY(), 400, 400});
			
			EndShader();

			FlipBuffer();
			Delay(16);
		}
	}


	Quit();
}

//#include <iostream>
//
//#include <SDL3/SDL.h>
//
//#include "../include/slib.h"
//
//using namespace sl;
//
//struct Vertex
//{
//	float x, y, z;
//	float r, g, b, a;
//};
//
//
//static Vertex vertices[]
//{
//	{0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f},     
//	{-0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f},   
//	{0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
//};
//
//
//struct UniformBuffer
//{
//	float time;
//};
//
//int main2()
//{
// 	if (!Init("shaders", 500, 500, 0))
//	{
//		goto quit;
//	}
//
//	{
//		SDL_GPUDevice* device = (SDL_GPUDevice*)GetDevice();
//		SDL_Window* window = (SDL_Window*)GetWindow();
//
//		//we need to first create vertex buffer, we supply the info using this struct:
//		SDL_GPUBufferCreateInfo vertexInfo = { 0 };
//		vertexInfo.size = sizeof(vertices);
//		vertexInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
//		SDL_GPUBuffer* vertexBuffer = SDL_CreateGPUBuffer(device, &vertexInfo);
//		if (!vertexBuffer)
//		{
//			std::cerr << "[ERROR] Failed to create vertex buffer: " << SDL_GetError() << '\n';
//			goto quit;
//		}
//		//then we need a transfer buffer to transfer the other buffer into the gpu
//		SDL_GPUTransferBufferCreateInfo transferInfo = {};
//		transferInfo.size = sizeof(vertices);
//		transferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
//		SDL_GPUTransferBuffer* transferBuffer = SDL_CreateGPUTransferBuffer(device, &transferInfo);
//		Vertex* transferData = (Vertex*)SDL_MapGPUTransferBuffer(device, transferBuffer, true);
//		SDL_memcpy(transferData, vertices, sizeof(vertices));
//		SDL_UnmapGPUTransferBuffer(device, transferBuffer);
//
//		SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(device);
//		SDL_GPUCopyPass* cpyPass = SDL_BeginGPUCopyPass(cmd);
//
//		SDL_GPUTransferBufferLocation transferLocation = { 0 };
//		transferLocation.offset = 0;
//		transferLocation.transfer_buffer = transferBuffer;
//
//		SDL_GPUBufferRegion vertexRegion = { 0 };
//		vertexRegion.buffer = vertexBuffer;
//		vertexRegion.offset = 0;
//		vertexRegion.size = sizeof(vertices);
//
//		SDL_UploadToGPUBuffer(cpyPass, &transferLocation, &vertexRegion, true);
//
//		SDL_EndGPUCopyPass(cpyPass);
//
//		SDL_SubmitGPUCommandBuffer(cmd);
//
//
//
//		while (!ShouldClose())
//		{
//
//			ClearBuffer(0, 0, 0, 255);
//
//
//			FlipBuffer();
//
//			
//
//			Delay(16);
//		}
//		
//		SDL_ReleaseGPUBuffer(device, vertexBuffer);
//		SDL_ReleaseGPUTransferBuffer(device, transferBuffer);
//	}
//
//quit:
//	Quit();
//
//	return 0;
//}
//
//int main()  
//{
//	if (!Init("static lib", 600, 600, SDL_WINDOW_RESIZABLE))
//	{
//		return 1;
//	}
//	SDL_GPUDevice* device = (SDL_GPUDevice*)GetDevice();
//	SDL_Window* window = (SDL_Window*)GetWindow();
//
//
//	SDL_GPUBufferCreateInfo bufferInfo{};
//	bufferInfo.size = sizeof(vertices);
//	bufferInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
//	SDL_GPUBuffer* vertexBuffer = SDL_CreateGPUBuffer(device, &bufferInfo);
//	if (!vertexBuffer)
//	{
//		std::cerr << "Failed to create vertex buffer: " << SDL_GetError() << '\n';
//		return 1;
//	}
//	std::cout << "[INFO] Created vertex buffer\n";
//
//
//	SDL_GPUTransferBufferCreateInfo transferInfo{};
//	transferInfo.size = sizeof(vertices);
//	transferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
//	SDL_GPUTransferBuffer* transferBuffer = SDL_CreateGPUTransferBuffer(device, &transferInfo);
//	if (!transferBuffer)
//	{
//		std::cerr << "Failed to create transfer buffer: " << SDL_GetError() << '\n';
//		return 1;
//	}
//	std::cout << "[INFO] Created transfer buffer\n";
//
//	Vertex* data = (Vertex*)SDL_MapGPUTransferBuffer(device, transferBuffer, false);
//	if (!data)
//	{
//		std::cerr << "Failed to map transfer buffer: " << SDL_GetError() << '\n';
//		return 1;
//	}
//	std::cout << "[INFO] Mapped transfer buffer\n";
//
//	SDL_memcpy(data, vertices, sizeof(vertices));
//
//	SDL_UnmapGPUTransferBuffer(device, transferBuffer);
//
//	SDL_GPUCommandBuffer* cmdBuffer = SDL_AcquireGPUCommandBuffer(device);
//	if (!cmdBuffer)
//	{
//		std::cerr << "Failed to acquite command buffer: " << SDL_GetError() << '\n';
//	}
//	std::cout << "[INFO] Acquired GPU command buffer\n";
//	SDL_GPUCopyPass* pass = SDL_BeginGPUCopyPass(cmdBuffer);
//	if (!pass)
//	{
//		std::cerr << "Failed to begin copy pass: " << SDL_GetError() << '\n';
//	}
//	std::cout << "[INFO] Begin copy pass";
//	SDL_GPUTransferBufferLocation location = { 0 };
//	location.transfer_buffer = transferBuffer;
//	location.offset = 0;
//
//	SDL_GPUSamplerCreateInfo SamplerInfo = {};
//
//
//	SDL_GPUBufferRegion region = { 0 };
//	region.buffer = vertexBuffer;
//	region.size = sizeof(vertices);
//	region.offset = 0;
//	SDL_UploadToGPUBuffer(pass, &location, &region, true);
//
//	SDL_EndGPUCopyPass(pass);
//	if (!SDL_SubmitGPUCommandBuffer(cmdBuffer))
//	{
//		std::cerr << "Failed to submit command buffer for copy: " << SDL_GetError() << '\n';
//	}
//	std::cout << "[INFO] Submitted GPU command buffer for copy\n";
//
//	size_t vertexCodeSize;
//	void* vertexCode = SDL_LoadFile("shaders/vertex.spv", &vertexCodeSize);
//
//	// create the vertex shader
//	SDL_GPUShaderCreateInfo vertexInfo{};
//	vertexInfo.code = (Uint8*)vertexCode; //convert to an array of bytes
//	vertexInfo.code_size = vertexCodeSize;
//	vertexInfo.entrypoint = "main";
//	vertexInfo.format = SDL_GPU_SHADERFORMAT_SPIRV; // loading .spv shaders
//	vertexInfo.stage = SDL_GPU_SHADERSTAGE_VERTEX; // vertex shader
//	vertexInfo.num_samplers = 0;
//	vertexInfo.num_storage_buffers = 0;
//	vertexInfo.num_storage_textures = 0;
//	vertexInfo.num_uniform_buffers = 0;
//	SDL_GPUShader* vertexShader = SDL_CreateGPUShader(device, &vertexInfo);
//
//	// free the file
//	SDL_free(vertexCode);
//
//	// create the fragment shader
//	size_t fragmentCodeSize;
//	void* fragmentCode = SDL_LoadFile("shaders/fragment.spv", &fragmentCodeSize);
//	if (!fragmentCode)
//	{
//		std::cerr << "Failed to load fragment shader file\n";
//	}
//	std::cout << "[INFO] Loaded fragment shader file\n";
//
//	// create the fragment shader
//	SDL_GPUShaderCreateInfo fragmentInfo{};
//	fragmentInfo.code = (Uint8*)fragmentCode;
//	fragmentInfo.code_size = fragmentCodeSize;
//	fragmentInfo.entrypoint = "main";
//	fragmentInfo.format = SDL_GPU_SHADERFORMAT_SPIRV;
//	fragmentInfo.stage = SDL_GPU_SHADERSTAGE_FRAGMENT; // fragment shader
//	fragmentInfo.num_samplers = 0;
//	fragmentInfo.num_storage_buffers = 0;
//	fragmentInfo.num_storage_textures = 0;
//	fragmentInfo.num_uniform_buffers = 1;
//
//	SDL_GPUShader* fragmentShader = SDL_CreateGPUShader(device, &fragmentInfo);
//	if (!fragmentShader)
//	{
//		std::cerr << "[ERROR] Failed to create fragment shader\n";
//	}
//	std::cout << "[INFO] Created fragment shader\n";
//		
//	// free the file
//	SDL_free(fragmentCode);
//
//	SDL_GPUGraphicsPipelineCreateInfo pipelineInfo{};
//
//	// bind shaders
//	pipelineInfo.vertex_shader = vertexShader;
//	pipelineInfo.fragment_shader = fragmentShader;
//
//	// draw triangles
//	pipelineInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
//
//	SDL_GPUVertexBufferDescription vertexBufferDescriptions[1];
//	vertexBufferDescriptions[0].slot = 0;
//	vertexBufferDescriptions[0].input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
//	vertexBufferDescriptions[0].instance_step_rate = 0;
//	vertexBufferDescriptions[0].pitch = sizeof(Vertex);
//
//	pipelineInfo.vertex_input_state.num_vertex_buffers = 1;
//	pipelineInfo.vertex_input_state.vertex_buffer_descriptions = vertexBufferDescriptions;
//
//
//	SDL_GPUVertexAttribute vertexAttributes[2];
//
//	// in_position
//	vertexAttributes[0].buffer_slot = 0; // fetch data from the buffer at slot 0
//	vertexAttributes[0].location = 0; // layout (location = 0) in shader
//	vertexAttributes[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3; //vec3
//	vertexAttributes[0].offset = 0; // start from the first byte from current buffer position
//
//	// in_color
//	vertexAttributes[1].buffer_slot = 0; // use buffer at slot 0
//	vertexAttributes[1].location = 1; // layout (location = 1) in shader
//	vertexAttributes[1].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4; // vec4
//	vertexAttributes[1].offset = sizeof(float) * 3; // 4th float from current buffer position
//
//	pipelineInfo.vertex_input_state.num_vertex_attributes = 2;
//	pipelineInfo.vertex_input_state.vertex_attributes = vertexAttributes;
//
//	// describe the color target
//	SDL_GPUColorTargetDescription colorTargetDescriptions[1];
//	colorTargetDescriptions[0] = {};
//	colorTargetDescriptions[0].format = SDL_GetGPUSwapchainTextureFormat(device, window);
//
//	pipelineInfo.target_info.num_color_targets = 1;
//	pipelineInfo.target_info.color_target_descriptions = colorTargetDescriptions;
//
//	SDL_GPUGraphicsPipeline* graphicsPipeline = SDL_CreateGPUGraphicsPipeline(device, &pipelineInfo);
//	if (!graphicsPipeline)
//	{
//		std::cerr << "[ERROR] Failed to create Graphics pipeline: " << SDL_GetError() << '\n';
//	}
//	std::cout << "[INFO] Created graphics pipeline\n";
//
//	while (!sl::ShouldClose())
//	{
//		while (sl::Ticking())
//		{
//			//fixed update goes here(does matter)
//
//			//
//		}
//		
//		//event handling goes here (doesnt't matter really, just don't put in fixed update)
//
//		//
//
//
//		SDL_GPUCommandBuffer* cmdBuf = SDL_AcquireGPUCommandBuffer((SDL_GPUDevice*)GetDevice());
//		if (!cmdBuf)
//		{
//			std::cerr << "Failed to acquire GPU command buffer: " << SDL_GetError() << '\n';
//			return 1;
//		}
//		SDL_GPUTexture* tex = nullptr;
//		if (!SDL_WaitAndAcquireGPUSwapchainTexture(cmdBuf, (SDL_Window*)GetWindow(), &tex, nullptr, nullptr))
//		{
//			std::cerr << "Failed to acquier GPU swap chain texture: " << SDL_GetError() << '\n';
//			return 1;
//		}
//
//		if (tex)
//		{
//			SDL_GPUColorTargetInfo targetInfo = { 0 };
//			targetInfo.texture = tex;
//			targetInfo.cycle = true;
//			targetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
//			targetInfo.store_op = SDL_GPU_STOREOP_STORE;
//			targetInfo.clear_color = { 0.f, 0.f, 0.f, 1.0f };
//
//			SDL_GPURenderPass* pass;
//			pass = SDL_BeginGPURenderPass(cmdBuf, &targetInfo, 1, nullptr);
//
//			SDL_BindGPUGraphicsPipeline(pass, graphicsPipeline);
//
//			SDL_GPUBufferBinding bufferBindings[1];
//			bufferBindings[0].buffer = vertexBuffer; // index 0 is slot 0 in this example
//			bufferBindings[0].offset = 0; // start from the first byte
//
//			SDL_BindGPUVertexBuffers(pass, 0, bufferBindings, 1); 
//			UniformBuffer time = { 0 };
//			time.time = CurrentTime();//SDL_GetTicks() / 1000.f; /// 1e9f; // the time since the app started in seconds
//			//std::cout << "Current Time: " << time.time << '\n';
//			SDL_PushGPUFragmentUniformData(cmdBuf, 0, &time, sizeof(UniformBuffer));
//
//			SDL_DrawGPUPrimitives(pass, 3, 1, 0, 0);
//			
//			SDL_EndGPURenderPass(pass);
//		}
//
//		if (!SDL_SubmitGPUCommandBuffer(cmdBuf))
//		{
//			std::cerr << "Failed to submit GPU command buffer: " << SDL_GetError() << '\n';
//		}
//
//
//
//		sl::Delay(16);
//	}
//
//	SDL_ReleaseGPUBuffer((SDL_GPUDevice*)GetDevice(), vertexBuffer);
//	SDL_ReleaseGPUTransferBuffer(device, transferBuffer);
//	SDL_ReleaseGPUShader(device, vertexShader);
//	SDL_ReleaseGPUShader(device, fragmentShader);
//
//	sl::Quit();
//}
