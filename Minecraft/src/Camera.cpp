#include "Camera.h"

Camera::Camera()
{
	RecalculateProjectionMatrix(90.0f, (float)1280 / (float)720);
	RecalculateViewMatrix();
}

Camera::Camera(float fov, float aspectRatio)
{
	RecalculateProjectionMatrix(fov, aspectRatio);
	RecalculateViewMatrix();
}


void Camera::SetPosition(const glm::vec3& position)
{
	m_Position = position;
	RecalculateViewMatrix();
}

void Camera::SetRotation(const glm::vec3& rotation)
{
	m_Rotation = rotation;
	RecalculateViewMatrix();
}

void Camera::RecalculateProjectionMatrix(float fov, float aspectRatio)
{
	m_ProjectionMatrix = glm::perspective((float)glm::radians(fov), aspectRatio, 0.1f, 1000.0f);
}

void Camera::RecalculateViewMatrix()
{
	glm::vec3 direction{};
	direction.x = cos(glm::radians(m_Rotation.x)) * cos(glm::radians(m_Rotation.y));
	direction.y = sin(glm::radians(m_Rotation.y));
	direction.z = sin(glm::radians(m_Rotation.x)) * cos(glm::radians(m_Rotation.y));
	direction = glm::normalize(direction);

	m_ViewMatrix = glm::lookAt(m_Position, m_Position + direction, glm::vec3(0, 1, 0));
}

