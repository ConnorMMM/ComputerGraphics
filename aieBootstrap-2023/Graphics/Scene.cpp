#include "Scene.h"
#include "Instance.h"

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
