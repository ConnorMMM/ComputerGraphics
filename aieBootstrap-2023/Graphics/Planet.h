#pragma once

#include "Gizmos.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <vector>

class Planet
{
public:
	Planet(const char* planetName, float distanceFromParent, float radius, float speed, glm::vec4 colour);
	~Planet();

	void Update(float deltaTime);
	void Draw();

	void AddRing(float innerRadius, float outerRadius, glm::vec4 ringColour);

	void AddChild(Planet* child);

	bool* Visible() { return &m_visible; }

	glm::vec3 GetPosition();
	std::vector<Planet*> GetPlanets();
	const char* GetName() { return m_planetName; }

	void SetParent(Planet* parent) { m_parent = parent; }
	glm::mat4 GetMatrix() { return m_matrix; }

protected:
	const char* m_planetName;

	std::vector<Planet*> m_children;
	Planet* m_parent;

	glm::mat4 m_matrix;
	float m_radius;
	float m_speed;
	float m_rotation;
	float m_distanceFromParent;

	glm::vec4 m_colour;

	bool m_hasRing;
	float m_ringInnerRadius;
	float m_ringOuterRadius;
	glm::vec4 m_ringColour;

	bool m_visible;

};