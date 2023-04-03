#pragma once
#include <glm/glm.hpp>
#include <string>

class Scene;
struct Light;

namespace aie {
	class OBJMesh;
	class ShaderProgram;
}

class Instance {
public:
	Instance(glm::mat4 transform, aie::OBJMesh* mesh, 
		aie::ShaderProgram* shader, std::string name, bool hasTexture);
	Instance(glm::vec3 position, glm::vec3 eulerAngles, glm::vec3 scale, 
		aie::OBJMesh* mesh, aie::ShaderProgram* shader, std::string name, bool hasTexture);
	~Instance() {};

	void Draw(Scene* scene);

	static glm::mat4 MakeTransform(glm::vec3 position, 
		glm::vec3 eulerAngles, glm::vec3 scale);

	void ImGui();

	// Getters
	glm::mat4 GetTransform() { return m_transform; }

	// Setters
	void SetTransform(glm::mat4 transform) { m_transform = transform; }

protected:
	glm::mat4 m_transform;
	aie::OBJMesh* m_mesh;
	aie::ShaderProgram* m_shader;

	bool m_visible = true;
	bool m_hasTexture;

	float m_curScale = 1;
	float m_prevScale = 1;

	glm::vec3 m_curRotation = glm::vec3(0);
	glm::vec3 m_prevRotation = glm::vec3(0);

	std::string m_name;
};

