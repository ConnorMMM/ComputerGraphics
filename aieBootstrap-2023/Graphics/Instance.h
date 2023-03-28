#pragma once
#include <glm/glm.hpp>

class Scene;
struct Light;

namespace aie {
	class OBJMesh;
	class ShaderProgram;
}

class Instance {
public:
	Instance(glm::mat4 transform, aie::OBJMesh* mesh, 
		aie::ShaderProgram* shader);
	Instance(glm::vec3 position, glm::vec3 eulerAngles, glm::vec3 scale, 
		aie::OBJMesh* mesh, aie::ShaderProgram* shader);
	~Instance() {};

	void Draw(Scene* scene);

	static glm::mat4 MakeTransform(glm::vec3 position, 
		glm::vec3 eulerAngles, glm::vec3 scale);

	void UpdateScale() { m_prevScale = m_curScale; }
	void UpdateRotation() { m_prevRotation = m_curRotation; }

	// Getters
	glm::mat4 GetTransform() { return m_transform; }
	float GetCurScale() { return m_curScale; }
	float GetPrevScale() { return m_prevScale; }
	glm::vec3 GetCurRotation() { return m_curRotation; }
	glm::vec3 GetPrevRotation() { return m_prevRotation; }

	// Setters
	void SetTransform(glm::mat4 transform) { m_transform = transform; }
	void SetPrevScale(float scale) { m_prevScale = scale; }
	void SetPrevRotation(glm::vec3 rotation) { m_prevRotation = rotation; }

	float* Position() { return &m_transform[3][0]; }
	bool* Visible() { return &m_visible; }
	float* CurScale() { return &m_curScale; }
	float* CurRotation() { return &m_curRotation[0]; }

protected:
	glm::mat4 m_transform;
	aie::OBJMesh* m_mesh;
	aie::ShaderProgram* m_shader;

	bool m_visible = true;

	float m_curScale = 1;
	float m_prevScale = 1;

	glm::vec3 m_curRotation = glm::vec3(0);
	glm::vec3 m_prevRotation = glm::vec3(0);
};

