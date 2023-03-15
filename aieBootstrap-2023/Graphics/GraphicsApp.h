#pragma once

#include "Application.h"
#include "Planet.h"
#include "Shader.h"
#include "Mesh.h"
#include "OBJMesh.h"

#include <glm/mat4x4.hpp>

#include <vector>

class GraphicsApp : public aie::Application {
public:

	GraphicsApp();
	virtual ~GraphicsApp();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

	void InitialisePlanets();

protected:
	
	bool LaunchShaders();
	void ImGUIRefresher();

	bool QuadLoader();
	void QuadDraw(glm::mat4 pvm);

	bool BunnyLoader();
	void BunnyDraw(glm::mat4 pvm);

	void PhongDraw(glm::mat4 pvm, glm::mat4 transform);

	bool SquareLoader();
	void SquareDraw(glm::mat4 pvm);

	bool CylinderLoader(float radius, float height, int segments);
	void CylinderDraw(glm::mat4 pvm);

	// camera transforms
	glm::mat4	m_viewMatrix;
	glm::mat4	m_projectionMatrix;

	Planet* m_sun;

	aie::ShaderProgram m_simpleShader;
	aie::ShaderProgram m_colorShader;
	aie::ShaderProgram m_phongShader;

	Mesh			   m_quadMesh;
	glm::mat4		   m_quadTransform;

	aie::OBJMesh	   m_bunnyMesh;
	glm::mat4		   m_bunnyTransform;

	struct Light {
		glm::vec3 direction;
		glm::vec3 color;
	};

	Light m_light;
	glm::vec3 m_ambientLight;

	Mesh m_squareMesh;
	glm::mat4 m_squareTransform;

	Mesh m_cylinderMesh;
	glm::mat4 m_cylinderTransform;

	bool m_quadHidden = true;
	bool m_bunnyHidden = true;
	bool m_squareHidden = true;
	bool m_cylinderHidden = true;
};