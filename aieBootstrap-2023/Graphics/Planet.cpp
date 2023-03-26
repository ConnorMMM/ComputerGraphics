#include "Planet.h"

using glm::vec3;
using glm::vec4;
using glm::mat4;
using aie::Gizmos;

Planet::Planet(const char* planetName, float distanceFromParent, float radius, float speed, glm::vec4 colour)
{
	m_planetName = planetName;

	m_distanceFromParent = distanceFromParent;
	m_radius = radius;
	m_speed = speed;
	m_rotation = 0;
	m_colour = colour;

	m_parent = nullptr;
	m_matrix = mat4(1);
	m_hasRing = false;

	m_visible = true;
}

Planet::~Planet()
{
}

void Planet::Update(float deltaTime)
{
	m_rotation += deltaTime * m_speed;

	if (m_parent)
		m_matrix[3] = glm::translate(glm::translate(mat4(1), m_parent->GetPosition()), 
			vec3(sin(m_rotation) * m_distanceFromParent, 0, cos(m_rotation) * m_distanceFromParent))[3];

	m_matrix = glm::rotate(m_matrix, deltaTime * m_speed * 0.2f, vec3(1, 0, 0));

	for each (Planet* child in m_children)
	{
		child->Update(deltaTime);
	}
}

void Planet::Draw()
{
	if (m_visible)
	{
		vec3 pos = GetPosition();

		Gizmos::addSphere(pos, m_radius, 8, 8, m_colour, &m_matrix);
		if (m_hasRing)
			Gizmos::addRing(pos, m_ringInnerRadius, m_ringOuterRadius, 9, m_ringColour, &m_matrix);
	}

	for each (Planet * child in m_children)
	{
		child->Draw();
	}
}

void Planet::AddRing(float innerRadius, float outerRadius, vec4 ringColour)
{
	m_hasRing = true;
	m_ringInnerRadius = innerRadius;
	m_ringOuterRadius = outerRadius;
	m_ringColour = ringColour;
}

void Planet::AddChild(Planet* child)
{
	m_children.push_back(child);
	child->SetParent(this);
}

vec3 Planet::GetPosition()
{
	vec3 pos = vec3(m_matrix[3][0], m_matrix[3][1], m_matrix[3][2]);

	return pos;
}

std::vector<Planet*> Planet::GetPlanets()
{
	std::vector<Planet*> planets;
	planets.push_back(this);
	for each (Planet* child in m_children)
	{
		std::vector<Planet*> temp = child->GetPlanets();
		for each (Planet* planet in temp)
		{
			planets.push_back(planet);
		}
	}
	return planets;
}
