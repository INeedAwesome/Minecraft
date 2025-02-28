#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera
{
public:
	Camera();
	Camera(float fov, float aspectRatio);
	~Camera() = default;

	void SetPosition(const glm::vec3& position);
	glm::vec3 GetPosition() { return m_Position; }

	void SetRotation(const glm::vec3& rotation);
	glm::vec3 GetRotation() { return m_Rotation; }

	void RecalculateProjectionMatrix(float fov, float aspectRatio);
	void RecalculateViewMatrix();

	glm::mat4 GetProjectionMatrix() { return m_ProjectionMatrix; }
	glm::mat4 GetViewMatrix() { return m_ViewMatrix; }

private:
	glm::vec3 m_Position = glm::vec3(0, 0, 0);
	glm::vec3 m_Rotation = glm::vec3(0, 0, 0);

	glm::mat4 m_ProjectionMatrix = glm::mat4(1);
	glm::mat4 m_ViewMatrix = glm::mat4(1);
};