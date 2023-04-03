#pragma once

#include "Gizmos.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <vector>
#include <string>

class Planet
{
public:
	Planet(const char* planetName, float distanceFromParent, 
		float radius, float orbitSpeed, glm::vec4 colour);
	~Planet();

	void Update(float deltaTime);
	void Draw();

	void AddRing(float innerRadius, float outerRadius, glm::vec4 ringColour);

	void AddChild(Planet* child);

	void ImGui();

	// Getters
	glm::vec3 GetPosition() { return m_transform[3]; }
	std::string GetName() { return m_planetName; }
	bool HasRing() { return m_hasRing; }

	// Setters
	void SetParent(Planet* parent) { m_parent = parent; }
	glm::mat4 GetTransform() { return m_transform; }

protected:
	std::string m_planetName;

	std::vector<Planet*> m_children;
	Planet* m_parent;

	glm::mat4 m_transform;
	float m_radius;
	float m_distanceFromParent;

	float m_orbitSpeed;
	float m_orbitAngle;

	float m_rotationSpeed;
	float m_rotation;

	glm::vec4 m_colour;

	bool m_hasRing;
	float m_ringInnerRadius;
	float m_ringOuterRadius;
	glm::vec4 m_ringColour;

	bool m_visible;

};