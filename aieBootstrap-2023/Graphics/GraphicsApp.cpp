#include "GraphicsApp.h"
#include "Gizmos.h"
#include "Input.h"

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <imgui.h>
#include <vector>

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
	m_viewMatrix = m_camera.GetViewMatrix();
	m_projectionMatrix = m_camera.GetProjectionMatrix(getWindowWidth(), getWindowHeight());
	
	InitialisePlanets();

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
	if (!m_planetsHidden)
		m_sun->Update(deltaTime);

	//Grab the time since the application has started
	float time = getTime();

	//Rotate time light to emulate a 'day/night' cycle
	m_light.direction = 
		glm::normalize(glm::vec3(glm::cos(time * 2), glm::sin(time * 2), 0));
	
	m_camera.Update(deltaTime);

	// quit if we press escape
	aie::Input* input = aie::Input::getInstance();
	if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
		quit();

	ImGUIRefresher();
}

void GraphicsApp::draw() {

	// wipe the screen to the background colour
	clearScreen();

	// update perspective based on screen size
	m_viewMatrix = m_camera.GetViewMatrix();
	m_projectionMatrix = m_camera.GetProjectionMatrix(getWindowWidth(), getWindowHeight());

	// Solar system
	if (!m_planetsHidden)
		m_sun->Draw();

	auto pv = m_projectionMatrix * m_viewMatrix;

	// Draw the quad setup in QuadLoader()
	if (!m_quadHidden)
		QuadTexturedDraw(pv * m_quadTransform);
		//QuadDraw(pv * m_quadTransform);

	// Draw the bunny setup in BunnyLoader()
	if(!m_bunnyHidden)
		PhongDraw(pv * m_bunnyTransform, m_bunnyTransform);
		//BunnyDraw(pv * m_bunnyTransform);

	if(!m_squareHidden)
		SquareDraw(pv * m_squareTransform);

	if (!m_cylinderHidden)
		CylinderDraw(pv * m_cylinderTransform);

	if (!m_pyramidHidden)
		PyramidDraw(pv * m_pyramidTransform);

	if (!m_sphereHidden)
		SphereDraw(pv * m_sphereTransform);

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
	//if (!QuadLoader())
	//	return false;
	if (!QuadTextureLoader())
		return false;
	
	// Used for loading in a OBJ bunny
	if (!BunnyLoader())
		return false;

	// Used for loading in a primitive square
	if (!SquareLoader())
		return false;

	// Used for loading in a primitive cylinder
	if (!CylinderLoader(1, 2, 6))
		return false;

	// Used for loading in a primitive pyramid
	if (!PyramidLoader(1, 2))
		return false;

	// Used for loading in a primitive sphere
	if (!SphereLoader(4, 2, 1))
		return false;

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
	if (ImGui::Button("Turn Planets ON/OFF", { 140, 20 }))
		m_planetsHidden = !m_planetsHidden;
	if (ImGui::Button("Turn Bunny ON/OFF", { 140, 20 }))
		m_bunnyHidden = !m_bunnyHidden;
	if (ImGui::Button("Turn Quad ON/OFF", { 140, 20 }))
		m_quadHidden = !m_quadHidden;
	if (ImGui::Button("Turn Square ON/OFF", { 140, 20 }))
		m_squareHidden = !m_squareHidden;
	if (ImGui::Button("Turn Cylinder ON/OFF", { 140, 20 }))
		m_cylinderHidden = !m_cylinderHidden;
	if (ImGui::Button("Turn Pyramid ON/OFF", { 140, 20 }))
		m_pyramidHidden = !m_pyramidHidden;
	if (ImGui::Button("Turn Sphere ON/OFF", { 140, 20 }))
		m_sphereHidden = !m_sphereHidden;
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

bool GraphicsApp::QuadTextureLoader()
{
	m_textureShader.loadShader(aie::eShaderStage::VERTEX,
		"./shaders/textured.vert");
	m_textureShader.loadShader(aie::eShaderStage::FRAGMENT,
		"./shaders/textured.frag");

	if (m_textureShader.link() == false)
	{
		printf("Textured Shader has an Error: %s\n",
			m_textureShader.getLastError());
		return false;
	}

	if (m_gridTexture.load("./textures/numbered_grid.tga") == false)
	{
		printf("Failed to load the grid texture correctly!\n");
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
void GraphicsApp::QuadTexturedDraw(glm::mat4 pvm)
{
	// Bind the shader
	m_textureShader.bind();

	// Bind the transform
	m_textureShader.bindUniform("ProjectionViewModel", pvm);

	// Bind the texture location
	m_textureShader.bindUniform("diffuseTexture", 0);

	//Bind the texture to a specific location
	m_gridTexture.bind(0);

	// Draw the quad using Mesh's draw
	m_quadMesh.Draw();
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

	// Defined as 2 vertices for each segment
	Mesh::Vertex* vertices = new Mesh::Vertex[segments * 2 + 2];

	vertices[0].position = { 0, 0,  0, 1 };
	vertices[1].position = { 0, height,  0, 1 };
	for (int i = 0; i < segments * 2; i++)
	{
		vertices[i + 2].position = { sin((PI * 2 / segments) * (i / 2)) * radius, 
			height * (i % 2),  cos((PI * 2 / segments) * (i / 2)) * radius, 1 };
	}

	unsigned int* indices = new unsigned int[segments * 12];

	int indicesIndex = 0;
	for (int i = 2; i < (segments * 2) + 2; i += 2)
	{
		int lastPoints = 0;
		if (i >= segments * 2)
			lastPoints = segments * 2;
		// First triangle (face)
		indices[indicesIndex] = i;
		indices[indicesIndex + 1] = i + 2 - lastPoints;
		indices[indicesIndex + 2] = i + 1;
		// Second triangle (face)
		indices[indicesIndex + 3] = i + 1;
		indices[indicesIndex + 4] = i + 2 - lastPoints;
		indices[indicesIndex + 5] = i + 3 - lastPoints;
		// Third triangle (top)
		indices[indicesIndex + 6] = i + 1;
		indices[indicesIndex + 7] = i + 3 - lastPoints;
		indices[indicesIndex + 8] = 1;
		// Fourth triangle (bottom)
		indices[indicesIndex + 9] = i + 2 - lastPoints;
		indices[indicesIndex + 10] = i;
		indices[indicesIndex + 11] = 0;
		indicesIndex += 12;
	}

	m_cylinderMesh.Initailise(segments * 2 + 2, vertices, segments * 12, indices);

	// This is a 10 'unit' wide square
	m_cylinderTransform = {
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

bool GraphicsApp::PyramidLoader(float width, float height)
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

	Mesh::Vertex vertices[5];
	vertices[0].position = { 0, height, 0, 1 };
	vertices[1].position = { -0.5f * width, 0,  0.5f * width, 1 };
	vertices[2].position = { 0.5f * width, 0,  0.5f * width, 1 };
	vertices[3].position = { -0.5f * width, 0, -0.5f * width, 1 };
	vertices[4].position = { 0.5f * width, 0, -0.5f * width, 1 };

	unsigned int indices[18] = { 4,3,2, 2,3,1, 1,2,0, 2,4,0, 4,3,0, 3,1,0};

	m_pyramidMesh.Initailise(5, vertices, 18, indices);

	// This is a 10 'unit' wide square
	m_pyramidTransform = {
		1, 0,  0,  0,
		0,  1, 0,  0,
		0,  0,  1, 0,
		0,  0,  0,  1
	};

	return true;
}
void GraphicsApp::PyramidDraw(glm::mat4 pvm)
{
	// Bind the shader
	m_simpleShader.bind();

	// Bind the transform
	m_simpleShader.bindUniform("ProjectionViewModel", pvm);

	// Draw the quad using Mesh's draw
	m_pyramidMesh.Draw();
}

bool GraphicsApp::SphereLoader(float segments, float rings, float radius)
{
	if (segments < 3 || rings < 2)
		return false;

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

	Mesh::Vertex* vertices = new Mesh::Vertex[segments * (rings - 1) + 2];
	int verticesIndex = 0;
	for (int r = 0; r <= rings; r++)
	{
		if (r == 0)
		{
			vertices[verticesIndex].position = { 0, radius, 0, 1 };
			verticesIndex++;
			continue;
		}
		if (r == rings)
		{
			vertices[verticesIndex].position = { 0, -radius, 0, 1 };
			verticesIndex++;
			continue;
		}

		float phi = ((PI * 2) / rings) * r;
		for (int s = 0; s < segments; s++)
		{
			float theta = ((PI * 2) / segments) * s;
			vertices[verticesIndex].position = { radius * sin(phi) * cos(theta), 
				radius * sin(phi) * sin(theta), radius * cos(phi), 1};
			verticesIndex++;
		}
	}

	unsigned int indices[24] = { 1,2,0, 2,3,0, 3,4,0, 4,1,0 };
	//unsigned int* indices = new unsigned int[((segments * 2) + ((rings - 2) * segments * 2)) * 3];
	/*int indicesIndex = 0;
	for (int s = 0; s < segments; s++)
	{
		for (int r = 0; r < rings; r++)
		{
			if (r == 0)
			{
				indices[indicesIndex] = s + 1;
				indices[indicesIndex + 1] = s + 2 > segments ? 1 : s + 2;
				indices[indicesIndex + 2] = 0;
				indicesIndex += 3;
				continue;
			}
			if (r == rings - 1)
			{
				indices[indicesIndex] = s + 2 > segments ? 1 + ((rings - 2) * segments) : s + 2 + ((rings - 2) * segments);
				indices[indicesIndex + 1] = s + 1 + ((rings - 2) * segments);
				indices[indicesIndex + 2] = ((rings - 2) * segments) + 1;
				indicesIndex += 3;
				continue;
			}

			indices[indicesIndex] = s + 1 + ((r - 1) * segments);
			indices[indicesIndex + 1] = s + 2 > segments ? 1 : s + 2 + ((r - 1) * segments);
			indices[indicesIndex + 2] = s + 1 + ((r - 2) * segments);
			indicesIndex += 3;
			indices[indicesIndex] = s + 2 > segments ? 1 : s + 2 + ((r - 2) * segments);
			indices[indicesIndex + 1] = s + 1 + ((r - 2) * segments);
			indices[indicesIndex + 2] = s + 2 > segments ? 1 : s + 2 + ((r - 1) * segments);
			indicesIndex += 3;
		}
	}*/

	m_sphereMesh.Initailise(segments * (rings - 1) + 2, vertices, ((segments * 2) + ((rings - 2) * segments * 2)) * 3, indices);

	// This is a 10 'unit' wide square
	m_sphereTransform = {
		1, 0,  0,  0,
		0,  1, 0,  0,
		0,  0,  1, 0,
		0,  0,  0,  1
	};

	return true;
}
void GraphicsApp::SphereDraw(glm::mat4 pvm)
{
	// Bind the shader
	m_simpleShader.bind();

	// Bind the transform
	m_simpleShader.bindUniform("ProjectionViewModel", pvm);

	// Draw the quad using Mesh's draw
	m_sphereMesh.Draw();
}
