#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "include/glad/glad.h"

#include "Shader.h"
#include "Buffers.h"

class Line 
{
public:
	Line() = default;
	void Create() {
		m_Vao.Create();
		m_Vbo.Create();
		m_LineColor = glm::vec3(1, 1, 1);

		const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
void main()
{
   gl_Position = projectionMatrix * viewMatrix * vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
)";
		const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
uniform vec3 color;
void main()
{
   FragColor = vec4(color, 1.0f);
}
)";

		m_Shader.Init(vertexShaderSource, fragmentShaderSource);
	}

	void SetData(const glm::vec3& start, const glm::vec3& end)
	{
		m_Vertices = {
			start.x, start.y, start.z,
			end.x, end.y, end.z,
		};

		m_Vao.Bind();
		m_Vbo.Bind();
		m_Vbo.Load(GL_ARRAY_BUFFER, sizeof(m_Vertices));
		m_Vbo.SetData(m_Vertices.data());

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		m_Vbo.Unbind();
		m_Vbo.Unbind();

	}

	void SetViewMatrix(const glm::mat4& view)
	{
		m_ViewMatrix = view;
	}

	void SetProjectionMatrix(const glm::mat4& projection)
	{
		m_ProjectionMatrix = projection;
	}

	int SetColor(glm::vec3 color) {
		m_LineColor = color;
		return 1;
	}

	int Draw() {
		m_Shader.Bind();
		m_Shader.SetUniform("viewMatrix", m_ViewMatrix);
		m_Shader.SetUniform("projectionMatrix", m_ProjectionMatrix);
		m_Shader.SetUniform("color", m_LineColor);
		
		m_Vao.Bind();
		m_Vbo.Bind();
		glLineWidth(1.0f);
		glDrawArrays(GL_LINES, 0, 2);
		return 1;

	}

private:
	Vao m_Vao;
	BufferObject m_Vbo;
	Shader m_Shader;
	std::vector<float> m_Vertices;
	glm::mat4 m_ViewMatrix;
	glm::mat4 m_ProjectionMatrix;
	glm::vec3 m_LineColor;
};
