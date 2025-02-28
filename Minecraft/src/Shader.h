#pragma once

#include <iostream>
#include <string>

#include "include/glad/glad.h"
#include <glm/glm.hpp>

#include <glm/gtc/type_ptr.hpp>

class Shader
{
public:
	Shader();
	Shader(const std::string& vertexSource, const std::string& fragmentSource);
	~Shader();

	void Init(const std::string& vertexSource, const std::string& fragmentSource);

	void Bind() { glUseProgram(m_ProgramID); }
	void Unbind() { glUseProgram(0); }

	void SetUniform(const std::string& name, float value);
	void SetUniform(const std::string& name, int value);
	void SetUniform(const std::string& name, unsigned int value);
	void SetUniform(const std::string& name, const glm::mat4& matrix);
	void SetUniform(const std::string& name, const glm::vec2& vec);
	void SetUniform(const std::string& name, const glm::vec3& vec);
	void SetUniform(const std::string& name, const glm::vec4& vec);

private:
	unsigned int m_ProgramID;
};