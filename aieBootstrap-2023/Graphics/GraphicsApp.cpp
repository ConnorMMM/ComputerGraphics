#include "GraphicsApp.h"
#include "Gizmos.h"
#include "Input.h"

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <imgui.h>

using glm::vec3;
using glm::vec4;
using glm::mat4;
using aie::Gizmos;

GraphicsApp::GraphicsApp() {

}

GraphicsApp::~GraphicsApp() {

}

bool GraphicsApp::startup() {
	
	setBackgroundColour(0.25f, 0.25f, 0.25f);

	// initialise gizmo primitive counts
	Gizmos::create(10000, 10000, 10000, 10000);

	// create simple camera transforms
	m_viewMatrix = glm::lookAt(vec3(10), vec3(0), vec3(0, 1, 0));
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, 16.0f / 9.0f, 0.1f, 1000.0f);

	//InitialisePlanets();

	m_light.color = { 1,1,1 };
	m_ambientLight = { 0.5,0.5,0.5 };

	return LaunchShaders();
}

void GraphicsApp::shutdown() {

	Gizmos::destroy();
}

void GraphicsApp::update(float deltaTime) {

	// wipe the gizmos clean for this frame
	Gizmos::clear();

	// draw a simple grid with gizmos
	vec4 white(1);
	vec4 black(0, 0, 0, 1);
	for (int i = 0; i < 21; ++i) {
		Gizmos::addLine(vec3(-10 + i, 0, 10),
						vec3(-10 + i, 0, -10),
						i == 10 ? white : black);
		Gizmos::addLine(vec3(10, 0, -10 + i),
						vec3(-10, 0, -10 + i),
						i == 10 ? white : black);
	}

	// add a transform so that we can see the axis
	Gizmos::addTransform(mat4(1));

	// Solar system
	//m_sun->Update(deltaTime);
	//m_sun->Draw();

	// quit if we press escape
	aie::Input* input = aie::Input::getInstance();

	//Grab the time since the application has started
	float time = getTime();

	//Rotate time light to emulate a 'day/night' cycle
	m_light.direction = 
		glm::normalize(glm::vec3(glm::cos(time * 2), glm::sin(time * 2), 0));
	

	if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
		quit();

	ImGUIRefresher();
}

void GraphicsApp::draw() {

	// wipe the screen to the background colour
	clearScreen();

	// update perspective based on screen size
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, 
		getWindowWidth() / (float)getWindowHeight(), 0.1f, 1000.0f);

	auto pv = m_projectionMatrix * m_viewMatrix;

	// Draw the quad setup in QuadLoader()
	if(!m_quadHidden)
		QuadDraw(pv * m_quadTransform);

	// Draw the bunny setup in BunnyLoader()
	//BunnyDraw(pv * m_bunnyTransform);
	if(!m_bunnyHidden)
		PhongDraw(pv * m_bunnyTransform, m_bunnyTransform);

	if(!m_squareHidden)
		SquareDraw(pv * m_squareTransform);

	if (!m_cylinderHidden)
		CylinderDraw(pv * m_cylinderTransform);

	Gizmos::draw(m_projectionMatrix * m_viewMatrix);
}

void GraphicsApp::InitialisePlanets()
{
	m_sun = new Planet(0, 2, 0, vec4(1, 0.75f, 0, 1));

	Planet* mercury = new Planet(1.5f, 0.3f, 1.60745f, vec4(0.3f, 0.2f, 0.5f, 1));
	m_sun->AddChild(mercury);
	Planet* venus = new Planet(1.9f, 0.3f, 1.17595f, vec4(0.8f, 0, 0, 1));
	m_sun->AddChild(venus);
	Planet* earth = new Planet(2.3f, 0.3f, 1, vec4(0, 0, 1, 1));
	m_sun->AddChild(earth);
	Planet* mars = new Planet(2.7f, 0.3f, 0.80849f, vec4(0.8f, 0.3f, 0.3f, 1));
	m_sun->AddChild(mars);

	Planet* jupiter = new Planet(3.4f, 0.8f, 0.43888f, vec4(0.8f, 0.7f, 0.6f, 1));
	m_sun->AddChild(jupiter);
	Planet* saturn = new Planet(4.3f, 0.7f, 0.32538f, vec4(0.8f, 0.75f, 0.75f, 1));
	saturn->AddRing(0.8f, 1.1f, vec4(1, 0.9f, 0.9f, 0.5f));
	m_sun->AddChild(saturn);

	Planet* uranus = new Planet(5.1f, 0.5f, 0.22867f, vec4(0, 0.8f, 0.8f, 1));
	uranus->AddRing(0.6f, 0.65f, vec4(0.8f, 0.8f, 1, 0.5f));
	m_sun->AddChild(uranus);
	Planet* neptune = new Planet(5.7f, 0.5f, 0.18233f, vec4(0, 0.7f, 1, 1));
	m_sun->AddChild(neptune);


	Planet* moon = new Planet(0.25f, 0.1f, 1.023f, vec4(0.9f, 0.9f, 1, 1));
	earth->AddChild(moon);
}

bool GraphicsApp::LaunchShaders()
{
	// Used for loading in a simple quad
	if (!m_quadHidden) {
		if (!QuadLoader())
			return false;
	}
	
	// Used for loading in a OBJ bunny
	if (!m_bunnyHidden) {
		if (!BunnyLoader())
			return false;
	}

	// Used for loading in a primitive square
	if (!m_squareHidden){
		if (!SquareLoader())
			return false;
	}

	// Used for loading in a primitive cylinder
	if (!m_cylinderHidden) {
		if (!CylinderLoader(1, 2, 6))
			return false;
	}

	return true;
}

void GraphicsApp::ImGUIRefresher()
{
	ImGui::Begin("Light Settings");
	ImGui::DragFloat3("Global Light Direction",
		&m_light.direction[0], 0.1, -1, 1);
	ImGui::DragFloat3("Global Light Color", 
		&m_light.color[0], 0.1, 0, 1);
	ImGui::End();

	ImGui::Begin("Primitive Settings");
	if (ImGui::Button("Turn Bunny ON/OFF", { 100, 20 }))
		m_bunnyHidden = !m_bunnyHidden;
	if (ImGui::Button("Turn Quad ON/OFF", { 100, 20 }))
		m_quadHidden = !m_quadHidden;
	if (ImGui::Button("Turn Square ON/OFF", { 100, 20 }))
		m_squareHidden = !m_squareHidden;
	if (ImGui::Button("Turn Cylinder ON/OFF", { 100, 20 }))
		m_cylinderHidden = !m_cylinderHidden;
	ImGui::End();
}

bool GraphicsApp::QuadLoader()
{
	m_simpleShader.loadShader(aie::eShaderStage::VERTEX,
		"./shaders/simple.vert");
	m_simpleShader.loadShader(aie::eShaderStage::FRAGMENT,
		"./shaders/simple.frag");

	if (m_simpleShader.link() == false)
	{
		printf("Simple Shader has an Error: %s\n",
			m_simpleShader.getLastError());
		return false;
	}

	// Defined as 4 vertices for the 2 triangles
	Mesh::Vertex vertices[4];
	vertices[0].position = { -0.5f, 0,  0.5f, 1 };
	vertices[1].position = { 0.5f, 0,  0.5f, 1 };
	vertices[2].position = { -0.5f, 0, -0.5f, 1 };
	vertices[3].position = { 0.5f, 0, -0.5f, 1 };

	unsigned int indices[6] = { 0,1,2,2,1,3 };

	m_quadMesh.Initailise(4, vertices, 6, indices);

	// This is a 10 'unit' wide quad
	m_quadTransform = {
		10, 0,  0,  0,
		0,  10, 0,  0,
		0,  0,  10, 0,
		0,  0,  0,  1
	};

	return true;
}
void GraphicsApp::QuadDraw(glm::mat4 pvm)
{
	// Bind the shader
	m_simpleShader.bind();

	// Bind the transform
	m_simpleShader.bindUniform("ProjectionViewModel", pvm);

	// Draw the quad using Mesh's draw
	m_quadMesh.Draw();
}

bool GraphicsApp::BunnyLoader()
{
	m_phongShader.loadShader(aie::eShaderStage::VERTEX, "./shaders/phong.vert");
	m_phongShader.loadShader(aie::eShaderStage::FRAGMENT, "./shaders/phong.frag");
	if (m_phongShader.link() == false)
	{
		printf("Color Shader Error: %s\n", m_phongShader.getLastError());
		return false;
	}

	if (m_bunnyMesh.load("./stanford/Bunny.obj") == false)
	{
		printf("Bunny Mesh Error!\n");
		return false;
	}

	m_bunnyTransform = {
		0.4f, 0,  0,  0,
		0,  0.4f, 0,  0,
		0,  0,  0.4f, 0,
		0,  0,  0,  1
	};

	return true;
}
void GraphicsApp::BunnyDraw(glm::mat4 pvm)
{
	// Bind the shader
	m_colorShader.bind();

	// Bind the transform
	m_colorShader.bindUniform("ProjectionViewModel", pvm);

	// Bind the color
	m_colorShader.bindUniform("BaseColor", vec4(1));

	// Draw the bunny using the Mesh's draw
	m_bunnyMesh.draw();
}

void GraphicsApp::PhongDraw(glm::mat4 pvm, glm::mat4 transform)
{
	// Bind the phong shader
	m_phongShader.bind();

	// Bind the camera position
	m_phongShader.bindUniform("CameraPosition",
		vec3(glm::inverse(m_viewMatrix)[3]));

	// Bind the directional light we defined
	m_phongShader.bindUniform("LightDirection", m_light.direction);
	m_phongShader.bindUniform("LightColor", m_light.color);
	m_phongShader.bindUniform("AmbientColor", m_ambientLight);

	// Bind the pvm using the one provided
	m_phongShader.bindUniform("ProjectionViewModel", pvm);

	// Bind the transform using the one provided
	m_phongShader.bindUniform("ModelMatrix", transform);

	// Draw the bunny using the Mesh's draw
	m_bunnyMesh.draw();
}

bool GraphicsApp::SquareLoader()
{
	m_simpleShader.loadShader(aie::eShaderStage::VERTEX,
		"./shaders/simple.vert");
	m_simpleShader.loadShader(aie::eShaderStage::FRAGMENT,
		"./shaders/simple.frag");

	if (m_simpleShader.link() == false)
	{
		printf("Simple Shader has an Error: %s\n",
			m_simpleShader.getLastError());
		return false;
	}

	// Defined as 8 vertices for the 12 triangles
	Mesh::Vertex vertices[8];
	vertices[0].position = { -0.5f, 0,  0.5f, 1 };
	vertices[1].position = { 0.5f, 0,  0.5f, 1 };
	vertices[2].position = { -0.5f, 0, -0.5f, 1 };
	vertices[3].position = { 0.5f, 0, -0.5f, 1 };

	vertices[4].position = { -0.5f, 1, 0.5f, 1 };
	vertices[5].position = { 0.5f, 1, 0.5f, 1 };
	vertices[6].position = { -0.5f, 1, -0.5f, 1 };
	vertices[7].position = { 0.5f, 1, -0.5f, 1 };

	unsigned int indices[36] = { 0,2,1, 1,2,3, 0,1,4, 4,1,5, 1,3,5, 5,3,7, 3,2,7, 7,2,6, 2,0,6, 6,0,4, 4,5,6, 6,5,7 };

	m_squareMesh.Initailise(8, vertices, 36, indices);

	// This is a 10 'unit' wide square
	m_squareTransform = {
		1, 0,  0,  0,
		0,  1, 0,  0,
		0,  0,  1, 0,
		0,  0,  0,  1
	};

	return true;
}
void GraphicsApp::SquareDraw(glm::mat4 pvm)
{
	// Bind the shader
	m_simpleShader.bind();

	// Bind the transform
	m_simpleShader.bindUniform("ProjectionViewModel", pvm);

	// Draw the quad using Mesh's draw
	m_squareMesh.Draw();
}

bool GraphicsApp::CylinderLoader(float radius, float height, int segments)
{
	m_simpleShader.loadShader(aie::eShaderStage::VERTEX,
		"./shaders/simple.vert");
	m_simpleShader.loadShader(aie::eShaderStage::FRAGMENT,
		"./shaders/simple.frag");

	if (m_simpleShader.link() == false)
	{
		printf("Simple Shader has an Error: %s\n",
			m_simpleShader.getLastError());
		return false;
	}

	// Defined as 8 vertices for the 12 triangles
	//Mesh::Vertex vertices[segments * 2];
	Mesh::Vertex vertices[8];


	vertices[0].position = { -0.5f, 0,  0.5f, 1 };
	vertices[1].position = { 0.5f, 0,  0.5f, 1 };
	vertices[2].position = { -0.5f, 0, -0.5f, 1 };
	vertices[3].position = { 0.5f, 0, -0.5f, 1 };

	vertices[4].position = { -0.5f, 1, 0.5f, 1 };
	vertices[5].position = { 0.5f, 1, 0.5f, 1 };
	vertices[6].position = { -0.5f, 1, -0.5f, 1 };
	vertices[7].position = { 0.5f, 1, -0.5f, 1 };

	unsigned int indices[36] = { 0,2,1, 1,2,3, 0,1,4, 4,1,5, 1,3,5, 5,3,7, 3,2,7, 7,2,6, 2,0,6, 6,0,4, 4,5,6, 6,5,7 };

	m_squareMesh.Initailise(8, vertices, 36, indices);

	// This is a 10 'unit' wide square
	m_squareTransform = {
		1, 0,  0,  0,
		0,  1, 0,  0,
		0,  0,  1, 0,
		0,  0,  0,  1
	};

	return true;
}

void GraphicsApp::CylinderDraw(glm::mat4 pvm)
{
	// Bind the shader
	m_simpleShader.bind();

	// Bind the transform
	m_simpleShader.bindUniform("ProjectionViewModel", pvm);

	// Draw the quad using Mesh's draw
	m_cylinderMesh.Draw();
}

