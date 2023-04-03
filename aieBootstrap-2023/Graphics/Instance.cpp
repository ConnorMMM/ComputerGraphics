#include "Instance.h"
#include <glm/ext.hpp>
#include "Scene.h"
#include "OBJMesh.h"
#include "Texture.h"
#include "BaseCamera.h"
#include "GraphicsApp.h"

#include <imgui.h>

Instance::Instance(glm::mat4 transform, aie::OBJMesh* mesh, aie::ShaderProgram* shader, 
	std::string name, bool hasTexture)
{
	m_transform = transform;
	m_mesh = mesh;
	m_shader = shader;

	m_name = name;
	m_hasTexture = hasTexture;
}

Instance::Instance(glm::vec3 position, glm::vec3 eulerAngles, glm::vec3 scale, 
	aie::OBJMesh* mesh, aie::ShaderProgram* shader, std::string name, bool hasTexture) :
	m_mesh(mesh), m_shader(shader), m_name(name), m_hasTexture(hasTexture)
{
	m_transform = MakeTransform(position, eulerAngles, scale);
}

void Instance::Draw(Scene* scene)
{
	if (!m_visible)
		return;

	// Set the shader pipeline
	m_shader->bind();

	// Bind all relevent uniforms for our shaders
	auto pvm = scene->GetCamera()->GetProjectionMatrix() * 
		scene->GetCamera()->GetViewMatrix() * m_transform;
	m_shader->bindUniform("ProjectionViewModel", pvm);
	m_shader->bindUniform("ModelMatrix", m_transform);

	m_shader->bindUniform("hasTexture", m_hasTexture);

	// Bind the camera position
	m_shader->bindUniform("CameraPosition", scene->GetCamera()->GetPosition());

	// Bind the point lights
	int numberofLights = scene->GetNumberOfLights();
	m_shader->bindUniform("numLights", numberofLights);
	m_shader->bindUniform("PointLightPositions", numberofLights, 
		scene->GetPointLightPositions());
	m_shader->bindUniform("PointLightColors", numberofLights, 
		scene->GetPointLightColors());

	// Bind the directional light we defined
	m_shader->bindUniform("LightDirection", scene->GetLight().direction);
	m_shader->bindUniform("LightColor", scene->GetLight().color);
	m_shader->bindUniform("AmbientColor", scene->GetAmbientLightColor());

	m_mesh->draw();
}

glm::mat4 Instance::MakeTransform(glm::vec3 position, glm::vec3 eulerAngles, glm::vec3 scale)
{
	return glm::translate(glm::mat4(1), position)
		* glm::rotate(glm::mat4(1), glm::radians(eulerAngles.z), glm::vec3(0, 0, 1))
		* glm::rotate(glm::mat4(1), glm::radians(eulerAngles.y), glm::vec3(0, 1, 0))
		* glm::rotate(glm::mat4(1), glm::radians(eulerAngles.x), glm::vec3(1, 0, 0))
		* glm::scale(glm::mat4(1), scale);
}

void Instance::ImGui()
{
	if (ImGui::CollapsingHeader(m_name.c_str()))
	{
		ImGui::Checkbox(("Toggle " + m_name).c_str(), &m_visible);
		ImGui::DragFloat3((m_name + ": Position").c_str(), &m_transform[3][0], .01);
		if (ImGui::DragFloat((m_name + ": Scale").c_str(), &m_curScale, .01, .01, 100))
		{
			float diff = m_curScale / m_prevScale;
			m_transform = glm::scale(m_transform, vec3(diff));
			m_prevScale = m_curScale;
		}
		if (ImGui::DragFloat3((m_name + ": Rotation").c_str(), &m_curRotation[0], .01))
		{
			vec3 diff = m_prevRotation - m_curRotation;
			m_transform = glm::rotate(m_transform, diff[0], vec3(1, 0, 0));
			m_transform = glm::rotate(m_transform, diff[1], vec3(0, 1, 0));
			m_transform = glm::rotate(m_transform, diff[2], vec3(0, 0, 1));
			m_prevRotation = m_curRotation;
		}
	}
}
