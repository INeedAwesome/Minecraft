#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX 
#include <Windows.h>

#include "World/World.h"
#include "Camera.h"
#include "Shader.h"
#include "Window.h"
#include "Player.h"
#include "Framebuffer.h"
#include "Textures/Texture2D.h"
#include "Line.h"

struct FrameData
{
	float FPS;
	float Frametime;
};

struct Crosshair 
{
	Texture2D texture;
	Shader shader;
	Vao vao;
	BufferObject vbo;
};

class Game
{
public:
	Game() = default;
	~Game() = default;

	bool Init();
	void Run();
	void Release();
	
private:
	void ProcessInput(float deltaTime);
	void Update(float deltaTime);
	void Render();
	void RenderImGui();

	void SetLineValues(const glm::ivec3 blockPosition);

private:
	bool m_Run = true;

	Window m_Window;
	World m_World;
	Player m_Player;
	Shader m_Shader;
	Framebuffer m_Framebuffer;

	Line line[12];

	Crosshair m_Crosshair;

	FrameData m_FrameData;

	bool m_ShowMenu = false;
	bool m_ShowDebugAllways = false;
	bool m_UseFog = true;

	bool m_DeleteChunks = true;
	int m_ViewDistance = 5;

	int m_CurrentBlock = 1;
	
};

