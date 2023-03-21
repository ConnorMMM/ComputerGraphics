#include "StationaryCamera.h"

StationaryCamera::StationaryCamera() : BaseCamera()
{
}

void StationaryCamera::SetRotation(glm::vec3 rotation)
{
	vec3 normRotation = glm::normalize(rotation);

	m_phi = glm::asin(rotation.y);
	m_theta = glm::acos(rotation.x / glm::cos(m_phi));

	m_viewTransform = glm::lookAt(m_position, m_position + normRotation, glm::vec3(0, 1, 0));
}
