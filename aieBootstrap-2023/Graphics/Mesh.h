#pragma once

#include <glm/glm.hpp>

class Mesh
{
public:

	Mesh() : m_triCount(0), m_vao(0), m_vbo(0), m_ibo(0) {}
	virtual ~Mesh();

	struct Vertex
	{
		glm::vec4 position;
		glm::vec4 normal;
		glm::vec2 texCoord;
	};

	void InitialiseQuad(); // Will be used to make a simple quad
	void InitialiseFullscreenQuad(); // Will be used for Post Processing
	
	void Initailise(unsigned int vertexCount, const Vertex* vertices,
		unsigned int indexCount = 0, unsigned int* indices = nullptr); // Will be used for implemeting a primitive

	virtual void Draw();

protected:

	unsigned int m_triCount;
	unsigned int m_vao; // The Vertex Array Object
	unsigned int m_vbo; // The Vertex Buffer Object
	unsigned int m_ibo; // The Index Buffer Object

};

