#include "StationaryCamera.h"

StationaryCamera::StationaryCamera() : BaseCamera()
{
}

void StationaryCamera::SetRotation(glm::vec3 rotation)
{
	m_phi = (float)rotation.x * 90;
	m_theta = (float)rotation.y * 90;
}
