#include "GraphicsApp.h"
#include "Gizmos.h"
#include "Input.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>

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

	//Solar system
	//CreatePlanets();

	//m_sun->Update(deltaTime);
	//m_sun->Draw();

	// quit if we press escape
	aie::Input* input = aie::Input::getInstance();

	if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
		quit();
}

void GraphicsApp::draw() {

	// wipe the screen to the background colour
	clearScreen();

	// update perspective based on screen size
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, 
		getWindowWidth() / (float)getWindowHeight(), 0.1f, 1000.0f);

#pragma region SimpleShader
	// Bind the shader
	m_simpleShader.bind();

	// Bind the transform

	auto pvm = m_projectionMatrix * m_viewMatrix * m_quadTransform;
	m_simpleShader.bindUniform("ProjectionViewModel", pvm);

	// Draw the quad using Mesh's draw
	m_quadMesh.Draw();

#pragma endregion

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

	m_quadMesh.InitialiseQuad();

	// This is a 10 'unit' wide quad
	m_quadTransform = {
		10, 0,  0,  0,
		0,  10, 0,  0,
		0,  0,  10, 0,
		0,  0,  0,  1
	};

	return true;
}
