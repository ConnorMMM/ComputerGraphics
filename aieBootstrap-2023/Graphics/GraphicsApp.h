#pragma once

#include "Application.h"
#include "Planet.h"
#include "Shader.h"
#include "Mesh.h"
#include "OBJMesh.h"
#include "SimpleCamera.h"
#include "FlyCamera.h"

#include "Scene.h"
#include "Instance.h"

#include <glm/mat4x4.hpp>

#include <vector>

#define PI 3.14159265359

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

	bool QuadTextureLoader();
	void QuadTexturedDraw(glm::mat4 pvm);

	bool BunnyLoader();
	void BunnyDraw(glm::mat4 pvm);
	void PhongDraw(glm::mat4 pvm, glm::mat4 transform);

	bool SpearLoader();
	bool KamaDaggerLoader();

	bool ObjLoader(aie::OBJMesh& objMesh, glm::mat4& transform,
		float scale, const char* filepath, bool flipTexture);
	bool ObjLoader(aie::OBJMesh& objMesh, glm::mat4& transform,
		float scale, const char* filepath, const char* filename, 
		bool flipTexture);

	// for textured OBJs
	void ObjDraw(glm::mat4 pv, glm::mat4 transform, aie::OBJMesh* objMesh, aie::ShaderProgram* shader);


	bool LoadPrimitiveShapes();
	void DrawPrimitiveShapes(glm::mat4 pv);
	
	bool SquareLoader();
	void SquareDraw(glm::mat4 pvm);

	bool CylinderLoader(float radius, float height, int segments);
	void CylinderDraw(glm::mat4 pvm);

	bool PyramidLoader(float width, float height);
	void PyramidDraw(glm::mat4 pvm);
	
	bool SphereLoader(float segments, float rings, float radius);
	void SphereDraw(glm::mat4 pvm);

	Scene*		m_scene;

	// camera transforms
	glm::mat4	m_viewMatrix;
	glm::mat4	m_projectionMatrix;

	aie::Texture m_gridTexture;

	Planet* m_sun;

	aie::ShaderProgram m_simpleShader;
	aie::ShaderProgram m_colorShader;
	aie::ShaderProgram m_phongShader;
	aie::ShaderProgram m_textureShader;
	aie::ShaderProgram m_normalLitShader;

	Mesh			   m_quadMesh;
	glm::mat4		   m_quadTransform;

	aie::OBJMesh	   m_bunnyMesh;
	glm::mat4		   m_bunnyTransform;

	aie::OBJMesh	   m_spearMesh;
	glm::mat4		   m_spearTransform;

	aie::OBJMesh	   m_kamadaggarMesh;
	glm::mat4		   m_kamadaggarTransform;

	Mesh			   m_squareMesh;
	glm::mat4		   m_squareTransform;

	Mesh			   m_cylinderMesh;
	glm::mat4		   m_cylinderTransform;

	Mesh			   m_pyramidMesh;
	glm::mat4		   m_pyramidTransform;
	
	Mesh			   m_sphereMesh;
	glm::mat4		   m_sphereTransform;

	FlyCamera*		   m_camera;

	/*struct Light {
		glm::vec3 direction;
		glm::vec3 color;
	};*/

	Light m_light;
	glm::vec3 m_ambientLight;

	bool m_planetsVisible = false;
	bool m_quadVisible = false;
	bool m_bunnyVisible = false;
	bool m_spearVisible = false;
	bool m_kamadaggarVisible = true;


	bool m_squareVisible = false;
	bool m_cylinderVisible = false;
	bool m_pyramidVisible = false;
	bool m_sphereVisible = false;
};