#include "StationaryCamera.h"

StationaryCamera::StationaryCamera() : BaseCamera()
{
}

void StationaryCamera::SetRotation(glm::vec3 rotation)
{
	m_phi = glm::degrees(rotation.y);
	m_theta = glm::degrees(rotation.x);
}
