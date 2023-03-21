#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <list>

class BaseCamera;
class Instance;

struct Light 
{
	Light() 
	{
		direction = glm::vec3(0);
		color = glm::vec3(1);
	}
	Light(glm::vec3 _position, glm::vec3 _color, float _intensity) 
	{
		direction = _position;
		color = _color * _intensity;
	}
	glm::vec3 direction;
	glm::vec3 color;
};

class Scene {
public:
	Scene(BaseCamera* camera, glm::vec2 windowSize,
		Light& light, glm::vec3 ambientLightColor);
	~Scene();

	void AddInstance(Instance* instance);
	void Draw();

	// Getters
	BaseCamera* GetCamera() { return m_camera; }
	glm::vec2 GetWindowSize();
	glm::vec3 GetAmbientLightColor() { return m_ambientLightColor; }
	Light GetLight() { return m_light; }

protected:
	BaseCamera* m_camera;
	glm::vec2 m_windowSize;
	Light m_light;
	glm::vec3 m_ambientLightColor;
	std::list<Instance*> m_instances;

};

