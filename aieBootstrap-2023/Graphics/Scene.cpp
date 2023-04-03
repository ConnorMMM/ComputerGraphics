#include "Scene.h"
#include "Instance.h"

#include "Gizmos.h"

Scene::Scene(BaseCamera* camera, glm::vec2 windowSize,
	Light& light, glm::vec3 ambientLightColor) : 
	m_camera(camera), m_windowSize(windowSize), m_light(light), 
	m_ambientLightColor(ambientLightColor)
{

}

Scene::~Scene()
{
	for (auto it = m_instances.begin(); it != m_instances.end(); it++)
	{
		delete* it;
	}
}

void Scene::AddInstance(Instance* instance)
{
	m_instances.push_back(instance);
}

void Scene::Draw()
{
	for (int i = 0; i < MAX_LIGHTS && i < m_pointLights.size(); i++)
	{
		m_pointLightPositions[i] = m_pointLights[i].direction;
		m_pointLightColors[i] = m_pointLights[i].color;

		float scale = glm::max(m_pointLights[i].color[0], glm::max(m_pointLights[i].color[1], m_pointLights[i].color[2]));
		glm::vec3 color;
		if (scale <= 1)
			color = m_pointLights[i].color;
		else
			color = glm::vec3(m_pointLights[i].color[0] / scale, m_pointLights[i].color[1] / scale, m_pointLights[i].color[2] / scale);
		aie::Gizmos::addSphere(m_pointLights[i].direction, 0.4f, 6, 8, glm::vec4(color, 1));
	}

	for (auto it = m_instances.begin(); it != m_instances.end(); it++)
	{
		Instance* instance = *it;
		instance->Draw(this);
	}
}

glm::vec2 Scene::GetWindowSize()
{
	return m_windowSize;
}
