#include "Player.h"

#include <iostream>

Player::Player()
	: m_PlayerWorld()
{
	m_Camera.SetPosition({ m_Position.x, m_Position.y, m_Position.z });
}

void Player::Update(float deltaTime)
{
	float cameraSpeed = 4.317f;
	if (Input::IsKeyPressed(VK_CONTROL))
		cameraSpeed = 7.127f;

	glm::vec3 movementDelta{};
	if (Input::IsKeyPressed('W'))
	{
		movementDelta.x += glm::cos(glm::radians(GetYaw()));
		movementDelta.z += glm::sin(glm::radians(GetYaw()));
	}
	if (Input::IsKeyPressed('S'))
	{
		movementDelta.x += -glm::cos(glm::radians(GetYaw()));
		movementDelta.z += -glm::sin(glm::radians(GetYaw()));
	}
	if (Input::IsKeyPressed('A'))
	{
		movementDelta.x += glm::sin(glm::radians(GetYaw()));
		movementDelta.z += -glm::cos(glm::radians(GetYaw()));
	}
	if (Input::IsKeyPressed('D'))
	{
		movementDelta.x += -glm::sin(glm::radians(GetYaw()));
		movementDelta.z += glm::cos(glm::radians(GetYaw()));
	}
	if (glm::length(movementDelta) > 0.0f)
	{
		movementDelta = glm::normalize(movementDelta) * cameraSpeed * deltaTime;
	}

	// Check if the player is on the ground 
	glm::vec3 belowPlayer = { GetPosition().x - playerSize.x, GetPosition().y + GRAVITY * deltaTime, GetPosition().z - playerSize.z };
	m_IsGrounded = IsGrounded(belowPlayer);

	if (!m_IsGrounded)
	{
		m_Velocity.y += (float)GRAVITY * deltaTime;
		m_Position.y += m_Velocity.y * deltaTime;
	}
	else 
	{
		m_Velocity.y = 0;
	}

	if (Input::IsKeyPressed(VK_SPACE) && m_IsGrounded) {
		m_Velocity.y = (float)glm::sqrt(1.25f * -2.0f * GRAVITY);
		m_Position.y += m_Velocity.y * deltaTime;
	}

	// Star represents the players x, z coord
	// X ----- X2
	// |       |
	// |   *   |
	// |       |
	// X1 ---- X

	// Check the bounding box from X1 to X2 

	glm::vec3 tempPlayerSize = { playerSize.x * 2, playerSize.y, playerSize.z * 2 };					// Calculate X2

	glm::vec3 newPosX = m_Position + glm::vec3(movementDelta.x, 0, 0);									// Handle X-axis 
	AABB newXAABB({ newPosX.x - playerSize.x, newPosX.y, newPosX.z - playerSize.z }, tempPlayerSize);	// Calculate X1's starting position
	if (!m_PlayerWorld->CheckCollision(newXAABB)) {
		m_Position.x = newPosX.x; // No collision, update X
	}

	glm::vec3 newPosZ = m_Position + glm::vec3(0, 0, movementDelta.z);									//  Handle Z-axis Sliding
	AABB newZAABB({ newPosZ.x - playerSize.x, newPosZ.y, newPosZ.z - playerSize.z }, tempPlayerSize);	// Calculate X1's starting position
	if (!m_PlayerWorld->CheckCollision(newZAABB)) {
		m_Position.z = newPosZ.z; // No collision, update Z
	}

	UpdateCamera();
}

void Player::SetYaw(float yaw)
{
	m_Rotation.x = yaw; 
	UpdateCamera(); 
}

void Player::SetPitch(float pitch)
{
	m_Rotation.y = pitch; 
	if (m_Rotation.y >= 89.0f)
		m_Rotation.y = 89.0f;
	if (m_Rotation.y <= -89.0f)
		m_Rotation.y = -89.0f;

	UpdateCamera();

}

void Player::UpdateCamera()
{
	m_Camera.SetPosition({ m_Position.x, m_Position.y + 1.8f, m_Position.z });
	m_Camera.SetRotation(m_Rotation);
}
