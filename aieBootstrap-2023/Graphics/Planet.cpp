#include "Planet.h"

#include <imgui.h>

using glm::vec3;
using glm::vec4;
using glm::mat4;
using aie::Gizmos;

Planet::Planet(const char* planetName, float distanceFromParent, 
	float radius, float orbitSpeed, glm::vec4 colour)
{
	m_planetName = planetName;

	m_distanceFromParent = distanceFromParent;
	m_radius = radius;
	
	m_orbitSpeed = orbitSpeed;
	m_orbitAngle = 0;

	m_rotationSpeed = orbitSpeed;
	m_rotation = 0;
	m_colour = colour;

	m_parent = nullptr;
	m_transform = mat4(1);

	m_hasRing = false;
	m_ringInnerRadius = 0;
	m_ringOuterRadius = 0;
	m_ringColour = colour;

	m_visible = true;
}

Planet::~Planet()
{
}

void Planet::Update(float deltaTime)
{
	m_rotation += deltaTime * m_orbitSpeed;

	if (m_parent)
		m_transform[3] = glm::translate(glm::translate(mat4(1), m_parent->GetPosition()),
			vec3(m_distanceFromParent * sin(m_rotation) * cos(m_orbitAngle),
				 m_distanceFromParent * sin(m_rotation) * sin(m_orbitAngle),
				 m_distanceFromParent * cos(m_rotation)))[3];

	m_transform = glm::rotate(m_transform, deltaTime * m_rotationSpeed * 0.2f, vec3(1, 0, 0));

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

		Gizmos::addSphere(pos, m_radius, 8, 8, m_colour, &m_transform);
		if (m_hasRing)
			Gizmos::addRing(pos, m_ringInnerRadius, m_ringOuterRadius, 9, m_ringColour, &m_transform);
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

void Planet::ImGui()
{
	if (ImGui::CollapsingHeader(m_planetName.c_str()))
	{
		ImGui::Checkbox(("Toggle " + m_planetName).c_str(), &m_visible);
		if (m_visible)
		{
			ImGui::SliderFloat4((m_planetName + ": Colour").c_str(), &m_colour[0], 0, 1);
			ImGui::DragFloat((m_planetName + ": Orbit Speed").c_str(), &m_orbitSpeed, .1f);
			ImGui::DragFloat((m_planetName + ": Orbit Angle").c_str(), &m_orbitAngle, .1f);
			ImGui::DragFloat((m_planetName + ": Rotation Speed").c_str(), &m_rotationSpeed, .1f);
			if (m_parent)
			{
				ImGui::DragFloat((m_planetName + ": Distance From " + m_parent->GetName()).c_str(), &m_distanceFromParent, .1f);
			}
			ImGui::Checkbox((m_planetName + ": Toggle Ring").c_str(), &m_hasRing);
			if (m_hasRing && ImGui::CollapsingHeader((m_planetName + ": Ring Settings").c_str()))
			{
				ImGui::DragFloat((m_planetName + ": Ring Inner Radius").c_str(), &m_ringInnerRadius, .1f);
				ImGui::DragFloat((m_planetName + ": Ring Outer Radius").c_str(), &m_ringOuterRadius, .1f);
				ImGui::SliderFloat4((m_planetName + ": Ring Colour").c_str(), &m_ringColour[0], 0, 1);
			}
		}
		for each (Planet* child in m_children)
		{
			child->ImGui();
		}
	}
}