#include "BaseCamera.h"

#include <glm/ext.hpp>
#include <imgui.h>

BaseCamera::BaseCamera()
{
	m_phi = 0;
	m_theta = 0;

	m_position = glm::vec3(0);
	m_viewTransform = glm::lookAt(glm::vec3(0), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0));
	GetWorldTransform();

	m_color = glm::vec4(.7f, .5f, 0, 1);
}

void BaseCamera::Update(float deltaTime)
{
	float thetaR = glm::radians(m_theta);
	float phiR = glm::radians(m_phi);
	glm::vec3 forward(glm::cos(phiR) * glm::cos(thetaR), glm::sin(phiR),
		glm::cos(phiR) * glm::sin(thetaR));

	m_viewTransform = glm::lookAt(m_position, m_position + forward, glm::vec3(0, 1, 0));
	m_worldTranform = glm::translate(glm::mat4(1), m_position) *
		glm::rotate(glm::mat4(1), glm::radians(forward.z), vec3(0, 0, 1)) *
		glm::rotate(glm::mat4(1), glm::radians(forward.y), vec3(0, 1, 0)) *
		glm::rotate(glm::mat4(1), glm::radians(forward.x), vec3(1, 0, 0));
}

void BaseCamera::Draw()
{
	float thetaR = glm::radians(m_theta);
	float phiR = glm::radians(m_phi);
	glm::vec3 forward(glm::cos(phiR) * glm::cos(thetaR), glm::sin(phiR),
		glm::cos(phiR) * glm::sin(thetaR));

	aie::Gizmos::addCylinderFilled(vec3(0), .25f, .25f, 8, m_color, &GetWorldTransform());
	aie::Gizmos::addLine(m_position, m_position + forward, glm::vec4(1, 0, 0, 1));
}

void BaseCamera::ImGui()
{
	ImGui::DragFloat3("Camera: Position", &m_position[0], .01);
	ImGui::DragFloat("Camera: X Rotation", &m_theta, .05);
	ImGui::DragFloat("Camera: Y Rotation", &m_phi, .05);
}

void BaseCamera::SetProjectionMatrix(float fieldOfView, float aspectRatio, float near, float far)
{
	m_aspectRatio = aspectRatio;
	m_projectionTransform = glm::perspective(fieldOfView, aspectRatio, near, far);
}
