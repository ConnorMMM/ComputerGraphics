#pragma once

#include "Gizmos.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <vector>

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


	Planet* GetParent() { return m_parent; }
	glm::vec3 GetPosition();
	std::vector<Planet*> GetPlanets();
	const char* GetName() { return m_planetName; }
	bool HasRing() { return m_hasRing; }

	void SetParent(Planet* parent) { m_parent = parent; }
	glm::mat4 GetMatrix() { return m_matrix; }

	bool* Visible() { return &m_visible; }
	float* Colour() { return &m_colour[0]; }
	float* OrbitSpeed() { return &m_orbitSpeed; }
	float* OrbitAngle() { return &m_orbitAngle; }
	float* RotationSpeed() { return &m_rotationSpeed; }
	float* DistanceFromParent() { return &m_distanceFromParent; }
	
	bool* Ring() { return &m_hasRing; }
	float* RingInnerRadius() { return &m_ringInnerRadius; }
	float* RingOuterRadius() { return &m_ringOuterRadius; }
	float* RingColour() { return &m_ringColour[0]; }

protected:
	const char* m_planetName;

	std::vector<Planet*> m_children;
	Planet* m_parent;

	glm::mat4 m_matrix;
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