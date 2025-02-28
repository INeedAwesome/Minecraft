#include "Game.h"

#include <chrono>

#include "include/glad/glad.h"
#include "Input.h"
#include "Textures/TextureAtlas.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "FileUtil.h"

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_win32.h"
#include "imgui/backends/imgui_impl_opengl3.h"

/*
* TODO: 
* Implement a framebuffer to read from when taking a screenshot
* Fix side of blocks - Kinda fixed grass block
* Water rendering - TODO: transparency
* Trees, and other decorations 
* Player Walking 
*/

bool Game::Init()
{
	if (!m_Window.Create(L"Minecraft 2", 1280, 720))
	{
		std::cout << "Could not create a window!" << std::endl;
		return false;
	}
	if (!m_Window.InitOpenGL())
	{
		std::cout << "Could not create an OpenGL context!" << std::endl;
		return false;
	}
	if (!Input::RegisterRawMouseInput())
	{
		std::cout << "Could not register raw mouse input! " << std::endl;
		std::cout << "Input Error: "<< GetLastError() << std::endl;

		return false;
	}

	TextureAtlas::Get().Bind();

	std::string vertexSource = FileUtil::ReadFile("assets/shaders/VertexShader.vs");
	std::string fragmentSource = FileUtil::ReadFile("assets/shaders/FragmentShader.fs");
	m_Shader.Init(vertexSource, fragmentSource);

	if (!m_Framebuffer.Init(m_Window.GetWidth(), m_Window.GetHeight()))
	{
		std::cout << "Could not create a framebuffer!" << std::endl;
		return false;
	}
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glEnable(GL_BLEND);

	// Crosshair initializiation
	static float verticesAndTexCoords[] =
	{
		-1.0f, -1.0f,		0.0f, 0.0f,
		 1.0f, -1.0f,		1.0f, 0.0f,
		 1.0f,  1.0f,		1.0f, 1.0f,

		-1.0f, -1.0f,		0.0f, 0.0f,
		 1.0f,  1.0f,		1.0f, 1.0f,
		-1.0f,  1.0f,		0.0f, 1.0f,
	};
	m_Crosshair.vao.Create();
	m_Crosshair.vbo.Create();
	m_Crosshair.vbo.Load(GL_ARRAY_BUFFER, sizeof(verticesAndTexCoords));
	m_Crosshair.vbo.SetData(verticesAndTexCoords);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	m_Crosshair.texture.Init("assets/crosshair.png");

	std::string vertexSourceCrosshair = FileUtil::ReadFile("assets/shaders/CrosshairVertexShader.vs");
	std::string fragmentSourceCrosshair = FileUtil::ReadFile("assets/shaders/CrosshairFragmentShader.fs");
	m_Crosshair.shader.Init(vertexSourceCrosshair, fragmentSourceCrosshair);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   // Enable Keyboard Controls

	// Setup Dear ImGui style
	//ImGui::StyleColorsDark();
	ImGui::StyleColorsClassic();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(m_Window.GetHWND());
	ImGui_ImplOpenGL3_Init();

	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	for (int i = 0; i < 12; i++)
		line[i].Create();

	m_Player.SetFov(90.0f);
	m_Player.SetPosition({ 0, 70, 0 });

	m_World.GenerateAroundPlayer(m_Player.GetPosition());

	return true;
}

void Game::Run()
{
	m_Window.CenterWindow();
	m_Window.Show();
	m_Window.SetMouseCaptured(true);

	std::chrono::high_resolution_clock::time_point current;
	std::chrono::nanoseconds deltaTimeNano;
	std::chrono::high_resolution_clock::time_point lastFrame = std::chrono::high_resolution_clock::now();
	float deltaTime = 0.0f;
	float isOneSecond = 0.0f;

	MSG msg;
	while (m_Run)
	{
		current = std::chrono::high_resolution_clock::now();
		deltaTimeNano = current - lastFrame;
		lastFrame = std::chrono::high_resolution_clock::now();
		deltaTime = deltaTimeNano.count() * 0.000000001f;
		m_FrameData.Frametime = deltaTime;
		isOneSecond += deltaTime;
		if (isOneSecond >= 1.0f)
		{
			std::wstring title = L"Minecraft 2 - FPS: ";
			title += std::to_wstring((int)(1 / deltaTime));
			m_FrameData.FPS = (1 / deltaTime);
			m_Window.SetTitle(title);
			isOneSecond = 0;
		}

		while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				m_Run = false;
			DispatchMessage(&msg);
			TranslateMessage(&msg);
		}

		Update(deltaTime);

		ProcessInput(deltaTime);

		if (m_Window.HasResized())
		{
			int width = m_Window.GetWidth();
			int height = m_Window.GetHeight();
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport(0, 0, width, height);

			m_Framebuffer.Update(width, height);
			glViewport(0, 0, width, height);

			m_Player.GetCamera().RecalculateProjectionMatrix(m_Player.GetFov(), (float)width / (float)height);
			m_Window.SetHasResized(false);
		}

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		RenderImGui();

		Render();
		
		ImGui::Render();
		ImGui::EndFrame();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		m_Window.Update();
	}
}

void Game::Release()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void Game::ProcessInput(float deltaTime)
{
	float cameraSpeed = 5.0f;
	float mouseSensitivity = 0.1f;
	
	if (Input::WasKeyPressed(VK_ESCAPE))
	{
		Input::ResetDeltas();
		m_Window.SetMouseCaptured(!m_Window.IsMouseCaptured());
		m_ShowMenu = !m_ShowMenu;
	}

	if (Input::WasKeyPressed(VK_F11))
		m_Window.SetFullscreen(!m_Window.IsFullscreen());

	if (!m_Window.IsMouseCaptured())
		return;
	
	m_CurrentBlock += Input::GetMouseScrollDelta();
	if (m_CurrentBlock >= BlockType::AMOUNT_BLOCKS)
		m_CurrentBlock = 0;
	else if (m_CurrentBlock < 0)
		m_CurrentBlock = BlockType::AMOUNT_BLOCKS - 1;

	if (Input::WasMousePressed(MOUSE_RIGHT))
	{
		Ray ray{};
		ray.origin = m_Player.GetPosition();
		ray.direction = m_Player.GetRotation();

		glm::ivec3 hitBlock{};
		glm::ivec3 hitNormal{};
		if (m_World.CastRay(ray, hitBlock, hitNormal, 10))
		{
			m_World.SetBlock(hitBlock + hitNormal, (BlockType)m_CurrentBlock);
		}
	}

	if (Input::WasMousePressed(MOUSE_LEFT))
	{
		Ray ray{};
		ray.origin = m_Player.GetPosition();
		ray.direction = m_Player.GetRotation();

		glm::ivec3 hitBlock{};
		glm::ivec3 hitNormal{};
		if (m_World.CastRay(ray, hitBlock, hitNormal, 10))
		{
			m_World.SetBlock(hitBlock, Air);
		}
	}
	
	if (Input::IsMousePressed(MOUSE_MIDDLE))
		cameraSpeed = 50.0f;

	if (Input::IsKeyPressed('W'))
		m_Player.AddMovement({ cameraSpeed * deltaTime * glm::cos(glm::radians(m_Player.GetYaw())), 0, cameraSpeed * deltaTime * glm::sin(glm::radians(m_Player.GetYaw())) });
	
	if (Input::IsKeyPressed('S'))
		m_Player.AddMovement({ -cameraSpeed * deltaTime * glm::cos(glm::radians(m_Player.GetYaw())), 0, -cameraSpeed * deltaTime * glm::sin(glm::radians(m_Player.GetYaw())) });
	
	if (Input::IsKeyPressed('A'))
		m_Player.AddMovement({cameraSpeed * deltaTime * glm::sin(glm::radians(m_Player.GetYaw())), 0, -cameraSpeed * deltaTime * glm::cos(glm::radians(m_Player.GetYaw())) });
	
	if (Input::IsKeyPressed('D'))
		m_Player.AddMovement({-cameraSpeed * deltaTime * glm::sin(glm::radians(m_Player.GetYaw())), 0, cameraSpeed * deltaTime * glm::cos(glm::radians(m_Player.GetYaw())) });
	
	if (Input::IsKeyPressed(VK_SHIFT))
		m_Player.AddMovement(cameraSpeed * -glm::normalize(glm::vec3(0, 1, 0)) * deltaTime);
	
	if (Input::IsKeyPressed(VK_SPACE))
		m_Player.AddMovement(cameraSpeed * glm::normalize(glm::vec3(0, 1, 0)) * deltaTime);

	if (Input::WasKeyPressed(VK_RETURN))
		m_Framebuffer.TakeScreenshot(m_Window.GetWidth(), m_Window.GetHeight());
	
	if (Input::IsKeyPressed('C'))
	{
		m_Player.SetFov(30.0f);
		m_Player.GetCamera().RecalculateProjectionMatrix(30.0f, (float)m_Window.GetWidth() / (float)m_Window.GetHeight());
	}
	else
	{
		m_Player.SetFov(90.0f);
		m_Player.GetCamera().RecalculateProjectionMatrix(90.0f, (float)m_Window.GetWidth() / (float)m_Window.GetHeight());
	}

	m_Player.SetYaw(m_Player.GetYaw() + (Input::GetMouseDeltaX() * mouseSensitivity));
	m_Player.SetPitch(m_Player.GetPitch() - (Input::GetMouseDeltaY() * mouseSensitivity));
	Input::ResetDeltas();
	m_Window.CenterCursor();

}

void Game::Update(float deltaTime)
{

	Ray ray{};
	ray.origin = m_Player.GetPosition();
	ray.direction = m_Player.GetRotation();

	glm::ivec3 hitBlock{};
	glm::ivec3 hitNormal{}; 
	if (m_World.CastRay(ray, hitBlock, hitNormal))
		SetLineValues(hitBlock);
	else 
		SetLineValues({ 0, 0, 0 });
	
	m_Player.Update(deltaTime);

	m_World.SetViewDistance(m_ViewDistance);
	m_World.Update(m_Player.GetPosition(), m_DeleteChunks);
	m_World.SortChunksForRendering(m_Player.GetPosition());
	m_World.GenerateAroundPlayer(m_Player.GetPosition());
}

void Game::Render()
{
	m_Framebuffer.Bind(); // Render to the framebuffer

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE); // Allow depth writing
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.2f, 0.6f, 1.0f, 1.0f);

	m_Shader.Bind();
	glActiveTexture(GL_TEXTURE0);
	TextureAtlas::Get().Bind();
	m_Shader.SetUniform("blockTexture", 0);
	m_Shader.SetUniform("view", m_Player.GetCamera().GetViewMatrix());
	m_Shader.SetUniform("projection", m_Player.GetCamera().GetProjectionMatrix());

	m_World.Render();
	m_Shader.Unbind();

	for (int i = 0; i < 12; i++)
		line[i].Draw();

	m_Crosshair.shader.Bind();
	m_Crosshair.vao.Bind();
	m_Crosshair.vbo.Bind();
	glActiveTexture(GL_TEXTURE3);
	m_Crosshair.texture.Bind();
	m_Crosshair.shader.SetUniform("crosshairTexture", 3);
	glm::vec2 res = { m_Window.GetWidth(), m_Window.GetHeight() };
	m_Crosshair.shader.SetUniform("iResolution", res);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	m_Crosshair.shader.Unbind();
	m_Crosshair.vao.Unbind();
	m_Crosshair.vbo.Unbind();
	m_Crosshair.texture.Unbind();

	// Render to the default framebuffer (the screen)
	m_Framebuffer.SetUniform("v_useFog", m_UseFog);
	m_Framebuffer.SetUniform("v_viewDistance", m_ViewDistance);
	m_Framebuffer.DrawToScreen(m_Player.GetCamera());
}

void Game::RenderImGui()
{
	if (m_ShowMenu || m_ShowDebugAllways)
	{
		ImGui::Begin("Minecraft 2 --- Debug");

		ImGui::Text("FPS	   : %d", (int)m_FrameData.FPS);
		ImGui::Text("Frametime : %f(ms)", m_FrameData.Frametime);
		ImGui::Spacing();
		ImGui::Checkbox("Running Game", &m_Run);
		ImGui::Checkbox("Show Debug Allways", &m_ShowDebugAllways);
		ImGui::Checkbox("Delete Chunks", &m_DeleteChunks);
		ImGui::Checkbox("Use Fog", &m_UseFog);
		ImGui::SliderInt("View Distance", &m_ViewDistance, 3, 30, nullptr, 0);

		ImGui::End();
	}


	std::string block;
	switch (m_CurrentBlock) {
		case Air: block += "Air"; break;
		case Stone: block += "Stone"; break;
		case Cobblestone: block += "Cobblestone"; break;
		case Dirt: block += "Dirt"; break;
		case GrassBlock: block += "GrassBlock"; break;
		case Water: block += "Water"; break;
		case Sand: block += "Sand"; break;
		case TreeLog: block += "TreeLog"; break;
		case Leaf: block += "Leaf"; break;
	default: break;
	}

	ImGui::Begin("Minecraft 2 - Block");
	ImGui::Text("Block is %s", block.c_str());
	ImGui::End();

	ImGui::Begin("Minecraft 2 - Controls");
	
	ImGui::Text("Escape - Menu");
	ImGui::Text("F11 - Fullscreen");
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Text("WASD - Moving");
	ImGui::Text("Shift - Move Down");
	ImGui::Text("Space - Move Up");
	ImGui::Spacing();
	ImGui::Text("Left Mouse - Destory block");
	ImGui::Text("Right Mouse - Place block");
	ImGui::Text("Middle Mouse Scroll - Switch block");
	ImGui::Text("Middle Mouse Press - Move fast");

	ImGui::End();

}

void Game::SetLineValues(const glm::ivec3 blockPos)
{
	// ChatGPT carries a bit too hard sometimes
	std::vector <glm::vec3> vertices = {
			glm::vec3(blockPos.x,		   blockPos.y,			blockPos.z),		  // Vertex 0
			glm::vec3(blockPos.x + 1.001f, blockPos.y,			blockPos.z),		  // Vertex 1
			glm::vec3(blockPos.x + 1.001f, blockPos.y + 1.001f, blockPos.z),		  // Vertex 2
			glm::vec3(blockPos.x,		   blockPos.y + 1.001f, blockPos.z),		  // Vertex 3
			glm::vec3(blockPos.x,		   blockPos.y,			blockPos.z + 1.001f), // Vertex 4
			glm::vec3(blockPos.x + 1.001f, blockPos.y,			blockPos.z + 1.001f), // Vertex 5
			glm::vec3(blockPos.x + 1.001f, blockPos.y + 1.001f, blockPos.z + 1.001f), // Vertex 6
			glm::vec3(blockPos.x,		   blockPos.y + 1.001f, blockPos.z + 1.001f)  // Vertex 7
	};

	// Define the 12 edges of the cube (pairs of vertex indices)
	std::vector <glm::ivec2> edges = {
		{0, 1}, {1, 2}, {2, 3}, {3, 0}, // Bottom face
		{4, 5}, {5, 6}, {6, 7}, {7, 4}, // Top face
		{0, 4}, {1, 5}, {2, 6}, {3, 7}  // Vertical edges
	};

	int i = 0;
	for (const auto& edge : edges) {
		glm::vec3 start = vertices[edge.x];
		glm::vec3 end = vertices[edge.y];

		line[i].SetData(start, end);
		line[i].SetViewMatrix(m_Player.GetCamera().GetViewMatrix());
		line[i].SetProjectionMatrix(m_Player.GetCamera().GetProjectionMatrix());
		line[i].SetColor({ 0, 0, 0 });
		i++;
	}

}



