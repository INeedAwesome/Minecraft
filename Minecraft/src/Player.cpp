#include "Player.h"

Player::Player()
{
	m_Camera.SetPosition({ m_Position.x, m_Position.y, m_Position.z });
}

void Player::Update(float deltaTime)
{
	//bool isGrounded = m_Position.y <= 50;
	//m_Velocity.y -= GRAVITY * deltaTime;
	//
	//if (isGrounded) 
	//{
	//	m_Velocity.y = 0.0f;
	//}
	//if (Input::IsKeyPressed(VK_SPACE) && isGrounded)
	//	m_Velocity.y = glm::sqrt(1.0f * 2.0f * GRAVITY);
	//m_Position.y += m_Velocity.y * deltaTime;

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
	m_Camera.SetPosition({ m_Position.x, m_Position.y, m_Position.z });
	m_Camera.SetRotation(m_Rotation);
}
