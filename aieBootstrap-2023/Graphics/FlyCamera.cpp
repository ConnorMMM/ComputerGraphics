#include "FlyCamera.h"

#include <glm/ext.hpp>
#include "Input.h"

FlyCamera::FlyCamera() : BaseCamera()
{
	m_speed = 1;
	m_minSpeed = 0.1f;
	m_maxSpeed = 6;

	m_turnSpeed = glm::radians(180.f);

	m_up = glm::vec3(0, 1, 0);
}

void FlyCamera::Update(float deltaTime)
{
	aie::Input* input = aie::Input::getInstance();
	float thetaR = glm::radians(m_theta);
	float phiR = glm::radians(m_phi);

	glm::vec3 forward(glm::cos(phiR) * glm::cos(thetaR), glm::sin(phiR),
		glm::cos(phiR) * glm::sin(thetaR));
	glm::vec3 right(-glm::sin(thetaR), 0, glm::cos(thetaR));
	m_up = glm::cross(right, forward);

	float shiftSpeed = 1;
	if (input->isKeyDown(aie::INPUT_KEY_LEFT_SHIFT))
		shiftSpeed = 3;

	if (input->getMouseScroll() != 0)
	{
		m_speed += input->getMouseScroll() * deltaTime;
		if (m_speed < m_minSpeed)
			m_speed = m_minSpeed;
		if (m_speed > m_maxSpeed)
			m_speed = m_maxSpeed;
	}

	// We will use WASD to move and the Q & E to go up and down
	if (input->isKeyDown(aie::INPUT_KEY_W))
		m_position += forward * m_speed * deltaTime * shiftSpeed;
	if (input->isKeyDown(aie::INPUT_KEY_S))
		m_position -= forward * m_speed * deltaTime * shiftSpeed;
	if (input->isKeyDown(aie::INPUT_KEY_A))
		m_position -= right * m_speed * deltaTime * shiftSpeed;
	if (input->isKeyDown(aie::INPUT_KEY_D))
		m_position += right * m_speed * deltaTime * shiftSpeed;

	if (input->isKeyDown(aie::INPUT_KEY_Q))
		m_position -= m_up * m_speed * deltaTime * shiftSpeed;
	if (input->isKeyDown(aie::INPUT_KEY_E))
		m_position += m_up * m_speed * deltaTime * shiftSpeed;

	// Get the mouse coordinates
	float mx = input->getMouseX();
	float my = input->getMouseY();

	// If the right button is held down, increment theta and phi (rotate)
	if (input->isMouseButtonDown(aie::INPUT_MOUSE_BUTTON_RIGHT))
	{
		m_theta += m_turnSpeed * (mx - m_lastMouse.x) * deltaTime;
		m_phi += m_turnSpeed * (my - m_lastMouse.y) * deltaTime;
	}

	m_lastMouse = glm::vec2(mx, my);

	BaseCamera::Update(deltaTime);
}
