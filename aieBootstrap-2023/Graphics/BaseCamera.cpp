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
}

void BaseCamera::SetProjectionMatrix(float fieldOfView, float aspectRatio, float near, float far)
{
	m_aspectRatio = aspectRatio;
	m_projectionTransform = glm::perspective(fieldOfView, aspectRatio, near, far);
}
