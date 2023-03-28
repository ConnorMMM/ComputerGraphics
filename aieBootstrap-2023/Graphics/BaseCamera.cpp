#include "BaseCamera.h"

#include <glm/ext.hpp>

BaseCamera::BaseCamera()
{
	m_phi = 0;
	m_theta = 0;

	m_position = glm::vec3(0);
	m_viewTransform = glm::lookAt(glm::vec3(0), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0));
}

void BaseCamera::Update(float deltaTime)
{
	float thetaR = glm::radians(m_theta);
	float phiR = glm::radians(m_phi);

	glm::vec3 forward(glm::cos(phiR) * glm::cos(thetaR), glm::sin(phiR),
		glm::cos(phiR) * glm::sin(thetaR));

	m_viewTransform = glm::lookAt(m_position, m_position + forward, glm::vec3(0, 1, 0));
}

void BaseCamera::Draw()
{
	float thetaR = glm::radians(m_theta);
	float phiR = glm::radians(m_phi);
	glm::vec3 forward(glm::cos(phiR) * glm::cos(thetaR), glm::sin(phiR),
		glm::cos(phiR) * glm::sin(thetaR));

	aie::Gizmos::addSphere(m_position, .25f, 6, 8, glm::vec4(.7f, .5f, 0, 1));
	aie::Gizmos::addLine(m_position, m_position + forward, glm::vec4(1, 0, 0, 1));
}

mat4 BaseCamera::GetWorldTransform()
{
	float thetaR = glm::radians(m_theta);
	float phiR = glm::radians(m_phi);
	glm::vec3 forward(glm::cos(phiR) * glm::cos(thetaR), glm::sin(phiR),
		glm::cos(phiR) * glm::sin(thetaR));

	return glm::translate(glm::mat4(1), m_position) *
		glm::rotate(glm::mat4(1), glm::radians(forward.z), vec3(0, 0, 1)) * 
		glm::rotate(glm::mat4(1), glm::radians(forward.y), vec3(0, 1, 0)) *
		glm::rotate(glm::mat4(1), glm::radians(forward.x), vec3(1, 0, 0));
}

void BaseCamera::SetProjectionMatrix(float fieldOfView, float aspectRatio, float near, float far)
{
	m_aspectRatio = aspectRatio;
	m_projectionTransform = glm::perspective(fieldOfView, aspectRatio, near, far);
}
