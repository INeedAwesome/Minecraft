#include "Shader.h"

Shader::Shader()
	: m_ProgramID(0)
{

}

Shader::Shader(const std::string& vertexSource, const std::string& fragmentSource)
	: m_ProgramID(0)
{
	Init(vertexSource, fragmentSource);
}

Shader::~Shader()
{
	glDeleteProgram(m_ProgramID);
}

void Shader::Init(const std::string& vertexSource, const std::string& fragmentSource)
{
	const char* vertexSrc = vertexSource.c_str();
	const char* fragmentSrc = fragmentSource.c_str();
	unsigned int vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertexShaderID, 1, &vertexSrc, 0);
	glShaderSource(fragmentShaderID, 1, &fragmentSrc, 0);
	glCompileShader(vertexShaderID);
	glCompileShader(fragmentShaderID);

	int success = 0;
	char infoLog[512] = { 0 };
	glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShaderID, 512, NULL, infoLog);
		std::cout << infoLog << std::endl;
	}
	glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShaderID, 512, NULL, infoLog);
		std::cout << infoLog << std::endl;
	}

	m_ProgramID = glCreateProgram();
	glAttachShader(m_ProgramID, vertexShaderID);
	glAttachShader(m_ProgramID, fragmentShaderID);
	glLinkProgram(m_ProgramID);

	glGetProgramiv(m_ProgramID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(m_ProgramID, 512, NULL, infoLog);
		std::cout << infoLog << std::endl;
	}
	glUseProgram(m_ProgramID);
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);
}

void Shader::SetUniform(const std::string& name, float value)
{
	int loc = glGetUniformLocation(m_ProgramID, name.c_str());
	glUniform1f(loc, value);
}
void Shader::SetUniform(const std::string& name, int value)
{
	int loc = glGetUniformLocation(m_ProgramID, name.c_str());
	glUniform1i(loc, value);
}
void Shader::SetUniform(const std::string& name, unsigned int value)
{
	int loc = glGetUniformLocation(m_ProgramID, name.c_str());
	glUniform1i(loc, value);
}
void Shader::SetUniform(const std::string& name, const glm::mat4& matrix)
{
	int loc = glGetUniformLocation(m_ProgramID, name.c_str());
	glUniformMatrix4fv(loc, 1, false, glm::value_ptr(matrix));
}
void Shader::SetUniform(const std::string& name, const glm::vec2& vec)
{
	int loc = glGetUniformLocation(m_ProgramID, name.c_str());
	glUniform2f(loc, vec.x, vec.y);
}
void Shader::SetUniform(const std::string& name, const glm::vec3& vec)
{
	int loc = glGetUniformLocation(m_ProgramID, name.c_str());
	glUniform3f(loc, vec.x, vec.y, vec.z);
}
void Shader::SetUniform(const std::string& name, const glm::vec4& vec)
{
	int loc = glGetUniformLocation(m_ProgramID, name.c_str());
	glUniform4f(loc, vec.x, vec.y, vec.z, vec.w);
}
