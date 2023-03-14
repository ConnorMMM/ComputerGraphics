#pragma once

#include "Gizmos.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <vector>

class Planet
{
public:

	Planet(float distanceFromParent, float radius, float speed, glm::vec4 colour);
	~Planet();

	void Update(float deltaTime);
	void Draw();

	void AddRing(float innerRadius, float outerRadius, glm::vec4 ringColour);

	void AddChild(Planet* child);

	glm::vec3 GetPosition();

	void SetParent(Planet* parent) { m_parent = parent; }
	glm::mat4 GetMatrix() { return m_matrix; }

protected:

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

};