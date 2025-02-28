#pragma once

#include "Camera.h"
#include "Input.h"

#define GRAVITY 15.82

class Player
{
public:
	Player();

	void Update(float deltaTime);

	void AddMovement(const glm::vec3& delta) { m_Position += delta; UpdateCamera(); }
	void AddRotation(const glm::vec3& delta) { m_Rotation += delta; UpdateCamera(); }

	void AddVelocity(const glm::vec3& velocity) { m_Velocity += velocity; UpdateCamera(); }
	void SetVelocity(const glm::vec3& velocity) { m_Velocity = velocity; UpdateCamera(); }
	glm::vec3 GetVelocity() const { return m_Velocity; }

	void SetPosition(const glm::vec3& position) { m_Position = position; UpdateCamera(); }
	glm::vec3 GetPosition() const { return m_Position; }

	void SetRotation(const glm::vec3& rotation) { m_Rotation = rotation; UpdateCamera(); }
	glm::vec3 GetRotation() const { return m_Rotation; }

	void SetYaw(float yaw);
	float GetYaw() const { return m_Rotation.x; }

	void SetPitch(float pitch);
	float GetPitch() const { return m_Rotation.y; }

	void SetFov(float fov) { m_Fov = fov; UpdateCamera(); };
	float GetFov() const { return m_Fov; }

	Camera& GetCamera() { return m_Camera; }

private:
	void UpdateCamera();

private:
	Camera m_Camera;
	glm::vec3 m_Position = glm::vec3(0, 0, 0);
	glm::vec3 m_Rotation = glm::vec3(0, 0, 0);

	float m_Fov = 90.0f;

	glm::vec3 m_Velocity = glm::vec3(0, 0, 0);
	glm::vec3 m_Acceleration = glm::vec3(0, 0, 0);


};

