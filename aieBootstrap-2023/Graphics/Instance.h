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

protected:
	glm::mat4 m_transform;
	aie::OBJMesh* m_mesh;
	aie::ShaderProgram* m_shader;

};

