#include "GraphicsApp.h"
#include "Gizmos.h"
#include "Input.h"

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <imgui.h>
#include <vector>
#include <string>

using glm::vec3;
using glm::vec4;
using glm::mat4;
using aie::Gizmos;

GraphicsApp::GraphicsApp() 
{

}

GraphicsApp::~GraphicsApp() 
{

}

bool GraphicsApp::startup() 
{
	setBackgroundColour(0.25f, 0.25f, 0.25f);

	// initialise gizmo primitive counts
	Gizmos::create(10000, 10000, 10000, 10000);

#pragma region CreateCameras
	// Create a user controlled fly camera
	m_flyCamera = new FlyCamera();
	m_flyCamera->SetProjectionMatrix(glm::pi<float>() * 0.25f, getWindowWidth(),
		getWindowHeight(), 0.1, 1000);
	m_flyCamera->SetColor(vec4(1, 1, 0, 1));

	// Create a stationary camera facing the front
	m_frontCamera = new StationaryCamera();
	m_frontCamera->SetPosition(vec3(-20, 0, 0));
	m_frontCamera->SetProjectionMatrix(glm::pi<float>() * 0.25f, getWindowWidth(),
		getWindowHeight(), 0.1, 1000);
	m_frontCamera->SetColor(vec4(1, 0, 0, 1));

	// Create a stationary camera facing the right
	m_rightCamera = new StationaryCamera();
	m_rightCamera->SetPosition(vec3(0, 0, 20));
	m_rightCamera->SetRotation(vec3(PI * 1.5f, 0, 0));
	m_rightCamera->SetProjectionMatrix(glm::pi<float>() * 0.25f, getWindowWidth(),
		getWindowHeight(), 0.1, 1000);
	m_rightCamera->SetColor(vec4(0, 0, 1, 1));

	// Create a stationary camera facing the top
	m_topCamera = new StationaryCamera();
	m_topCamera->SetPosition(vec3(0, 20, 0));
	m_topCamera->SetRotation(vec3(0, PI * 1.5f, 0));
	m_topCamera->SetProjectionMatrix(glm::pi<float>() * 0.25f, getWindowWidth(),
		getWindowHeight(), 0.1, 1000);
	m_topCamera->SetColor(vec4(0, 1, 0, 1));

	// Sets the current camera to the user controlled fly camera
	m_curCamera = m_flyCamera;
#pragma endregion

	m_viewMatrix = m_curCamera->GetViewMatrix();
	m_projectionMatrix = m_curCamera->GetProjectionMatrix();
	
	InitialisePlanets();
	
	m_ambientLight = { 0.5, 0.5, 0.5 };

	Light light;
	light.color = { 1, 1, 1 };
	light.direction = { 1, -1, 1 };

	m_emitter = new ParticleEmitter();
	m_emitter->Initialise(1000, 500, .1f, 1.0f, .5f, 2.5f, .5f, .05f,
		glm::vec4(0, 0, 1, 1), glm::vec4(0, 1, 0, 1));

#pragma region CreateScene
	m_scene = new Scene(m_curCamera, glm::vec2(getWindowWidth(),
		getWindowHeight()), light, m_ambientLight);

	m_scene->AddPointLight(vec3(0), vec3(0, 1, 0), 50);
	m_scene->AddPointLight(vec3(5, 3, 0), vec3(1, 0, 0), 50);
	m_scene->AddPointLight(vec3(-5, 3, 0), vec3(0, 0, 1), 50);
#pragma endregion

	return LaunchShaders();
}

void GraphicsApp::shutdown() 
{
	Gizmos::destroy();
	delete m_scene;
}

void GraphicsApp::update(float deltaTime) 
{
	m_scene->SetCamera(m_curCamera);
	
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
	if (m_planetsVisible)
		m_sun->Update(deltaTime);

	//Grab the time since the application has started
	float time = getTime();

#pragma region Lights
	//Rotate time light to emulate a 'day/night' cycle
	m_light.direction = 
		glm::normalize(glm::vec3(glm::cos(time * 2), glm::sin(time * 2), 0));

	float scale = 2 / (3 - cos(2 * time));
	float lightX = scale * cos(time) * 2;
	float lightZ = scale * sin(2 * time) / 2 * 2;
	m_scene->SetPointLightPos(0, glm::vec3(lightX, 0, lightZ));

#pragma endregion
	
#pragma region ParticleEmitter
	m_emitter->SetPosition(m_scene->GetPointLightPos(0));
	m_emitter->SetColor(glm::vec4(m_scene->GetPointLightColor(0), 1));
	m_emitter->Update(deltaTime, m_curCamera->GetWorldTransform());
#pragma endregion

	if (FlyCamera* flyCamera = dynamic_cast<FlyCamera*>(m_curCamera))
		flyCamera->Update(deltaTime);
	else
		m_curCamera->Update(deltaTime);

	// quit if we press escape
	aie::Input* input = aie::Input::getInstance();
	if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
		quit();


	ImGUIRefresher();
}

void GraphicsApp::draw() {
	// Bind the render target as the first part of our draw function
	m_renderTarget.bind();

	// wipe the screen to the background colour
	clearScreen();

	// update perspective based on screen size
	m_viewMatrix = m_curCamera->GetViewMatrix();
	m_projectionMatrix = m_curCamera->GetProjectionMatrix();

#pragma region DrawCameras
	m_flyCamera->Draw();
	m_frontCamera->Draw();
	m_rightCamera->Draw();
	m_topCamera->Draw();
#pragma endregion

#pragma region DrawSolarSystem
	if (m_planetsVisible)
		m_sun->Draw();
#pragma endregion

	auto pv = m_projectionMatrix * m_viewMatrix;
	
#pragma region DrawPrimitiveShapes
	if (m_primitiveShapesVisible)
	{
		if (m_squareVisible)
			PrimitiveShapeDraw(pv * m_squareTransform, &m_squareMesh);

		if (m_cylinderVisible)
			PrimitiveShapeDraw(pv * m_cylinderTransform, &m_cylinderMesh);

		if (m_pyramidVisible)
			PrimitiveShapeDraw(pv * m_pyramidTransform, &m_pyramidMesh);

		if (m_sphereVisible)
			PrimitiveShapeDraw(pv * m_sphereTransform, &m_sphereMesh);
	}
#pragma endregion

#pragma region DrawModels
	if (m_modelsVisible)
	{
		m_scene->Draw();

		// Draw the bunny setup in BunnyLoader()
		//if (m_bunnyVisible)
		//	ObjDraw(pv, m_bunnyTransform, &m_bunnyMesh, &m_phongShader);
	}
#pragma endregion

	m_particleShader.bind();
	m_particleShader.bindUniform("ProjectionViewModel", pv * m_particleEmitTransform);
	m_emitter->Draw();

	Gizmos::draw(m_projectionMatrix * m_viewMatrix);

	// Unbind the target to return to the back
	m_renderTarget.unbind();

	clearScreen();

	// Bind the post process shader and the texture
	m_postProcessShader.bind();
	m_postProcessShader.bindUniform("colorTarget", 0);
	m_postProcessShader.bindUniform("postProcessTarget", m_postProcessTarget);
	m_postProcessShader.bindUniform("windowWidth", (int)getWindowWidth());
	m_postProcessShader.bindUniform("windowHeight", (int)getWindowHeight());
	m_postProcessShader.bindUniform("time", getTime());
	m_renderTarget.getTarget(0).bind(0);

	m_fullScreenQuad.Draw();
}

void GraphicsApp::InitialisePlanets()
{
	m_sun = new Planet("Sun", 0, 2, 0, vec4(1, 0.75f, 0, 1));

	m_sun->AddChild(new Planet("Mercury", 1.5f, 0.3f, 1.60745f, vec4(0.3f, 0.2f, 0.5f, 1)));
	m_sun->AddChild(new Planet("Venus", 1.9f, 0.3f, 1.17595f, vec4(0.8f, 0, 0, 1)));
	Planet* earth = new Planet("Earth", 2.3f, 0.3f, 1, vec4(0, 0, 1, 1));
	m_sun->AddChild(earth);
	m_sun->AddChild(new Planet("Mars", 2.7f, 0.3f, 0.80849f, vec4(0.8f, 0.3f, 0.3f, 1)));
	m_sun->AddChild(new Planet("Jupiter", 3.4f, 0.8f, 0.43888f, vec4(0.8f, 0.7f, 0.6f, 1)));
	
	Planet* saturn = new Planet("Saturn", 4.3f, 0.7f, 0.32538f, vec4(0.8f, 0.75f, 0.75f, 1));
	saturn->AddRing(0.8f, 1.1f, vec4(1, 0.9f, 0.9f, 0.5f));
	m_sun->AddChild(saturn);
	Planet* uranus = new Planet("Uranus", 5.1f, 0.5f, 0.22867f, vec4(0, 0.8f, 0.8f, 1));
	uranus->AddRing(0.6f, 0.65f, vec4(0.8f, 0.8f, 1, 0.5f));
	m_sun->AddChild(uranus);
	m_sun->AddChild(new Planet("Neptune", 5.7f, 0.5f, 0.18233f, vec4(0, 0.7f, 1, 1)));

	earth->AddChild(new Planet("Earth's Moon", 0.25f, 0.1f, 1.023f, vec4(0.9f, 0.9f, 1, 1)));
}

bool GraphicsApp::LaunchShaders()
{
	if (m_renderTarget.initialise(1, getWindowWidth(),
		getWindowHeight()) == false)
	{
		printf("Render Target Error!\n");
		return false;
	}

#pragma region LoadingShaders
	// Textured Mesh Shader
	m_normalLitShader.loadShader(aie::eShaderStage::VERTEX,
		"./shaders/normalLit.vert");
	m_normalLitShader.loadShader(aie::eShaderStage::FRAGMENT,
		"./shaders/normalLit.frag");
	if (m_normalLitShader.link() == false)
	{
		printf("Normal Lit Phong Shader Error: %s\n",
			m_normalLitShader.getLastError());
		return false;
	}

	// Untextured Mesh Shader
	m_phongShader.loadShader(aie::eShaderStage::VERTEX, "./shaders/phong.vert");
	m_phongShader.loadShader(aie::eShaderStage::FRAGMENT, "./shaders/phong.frag");
	if (m_phongShader.link() == false)
	{
		printf("Color Shader Error: %s\n", m_phongShader.getLastError());
		return false;
	}

	// Simple Untextured Mesh Shader
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

	// Post Processing Shader
	m_postProcessShader.loadShader(aie::eShaderStage::VERTEX, "./shaders/post.vert");
	m_postProcessShader.loadShader(aie::eShaderStage::FRAGMENT, "./shaders/post.frag");
	if (m_postProcessShader.link() == false)
	{
		printf("Post Process Shader Error: %s\n", m_postProcessShader.getLastError());
		return false;
	}

	// Particle Shader
	m_particleShader.loadShader(aie::eShaderStage::VERTEX, "./shaders/particle.vert");
	m_particleShader.loadShader(aie::eShaderStage::FRAGMENT, "./shaders/particle.frag");
	if (m_particleShader.link() == false)
	{
		printf("Particle Shader Error: %s\n", m_particleShader.getLastError());
		return false;
	}
#pragma endregion

	m_particleEmitTransform = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	// Used for loading in a simple quad
	if (!QuadTextureLoader())
		return false;

	// Create a full screen quad
	m_fullScreenQuad.InitialiseFullscreenQuad();
	
#pragma region LoadingOBJMeshes
	// Used for loading in a OBJ bunny
	//if (!BunnyLoader())
	//	return false;
	if (!ObjLoader(m_bunnyMesh, m_bunnyTransform, .1,
		"./stanford/Bunny.obj", "Bunny", true))
		return false;

	// Used for loading in a OBJ spear
	if (!ObjLoader(m_spearMesh, m_spearTransform, 1,
		"./soulspear/soulspear.obj", "Spear", true))
		return false;

	// Used for loading in a OBJ kama dagger
	if (!ObjLoader(m_kamadaggarMesh, m_kamadaggarTransform, 0.005,
		"./kamadagger/kamadagger.obj", "Kama Dagger", true))
		return false;
#pragma endregion

#pragma region LoadingPrimitiveMeshes
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
	if (!SphereLoader(9, 6, 1))
		return false;
#pragma endregion
	
#pragma region InstanceOBJs
	// Spear Model
	m_scene->AddInstance(new Instance(m_spearTransform, &m_spearMesh, &m_normalLitShader, "Soul Spear", true));
	// Kama Dagger Model
	m_scene->AddInstance(new Instance(m_kamadaggarTransform, &m_kamadaggarMesh, &m_normalLitShader, "Kama Dagger", true));
	// Bunny Model
	m_scene->AddInstance(new Instance(m_bunnyTransform, &m_bunnyMesh, &m_normalLitShader, "Bunny", false));

#pragma endregion

	return true;
}

void GraphicsApp::ImGUIRefresher()
{
	ImGui::Begin("Light Settings");
	int numOfLights = m_scene->GetNumberOfLights();
	for (int i = 0; i < numOfLights; i++)
	{
		std::string name = "Light " + std::to_string(i + 1);
		if (ImGui::CollapsingHeader(name.c_str()))
		{
			vec3 lightPos = m_scene->GetPointLightPos(i);
			if (ImGui::DragFloat3((name + ": Position").c_str(), &lightPos[0], 0.1f))
			{
				m_scene->SetPointLightPos(i, lightPos);
			}
			vec3 lightColor = m_scene->GetPointLightColor(i);
			if (ImGui::DragFloat3((name + ": Color").c_str(), &lightColor[0], 0.1f))
			{
				m_scene->SetPointLightColor(i, lightColor);
			}
		}
	}
	ImGui::End();

	ImGui::Begin("Toggle Settings");
	ImGui::Checkbox("Toggle Planets", &m_planetsVisible);
	ImGui::Checkbox("Toggle Primitive Shapes", &m_primitiveShapesVisible);
	ImGui::Checkbox("Toggle Models", &m_modelsVisible);
	//ImGui::Checkbox("Toggle Quad", &m_quadVisible);
	ImGui::Checkbox("Toggle Particle System", m_emitter->Visible());
	ImGui::End();

	ImGui::Begin("Camera Settings");
	if (ImGui::CollapsingHeader("Cameras"))
	{
		if (ImGui::Button("Fly Camera", { 180, 30 }))
		{
			m_curCamera = m_flyCamera;
		}
		if (ImGui::Button("Front Stationary Camera", { 180, 30 }))
		{
			m_curCamera = m_frontCamera;
		}
		if (ImGui::Button("Right Stationary Camera", { 180, 30 }))
		{
			m_curCamera = m_rightCamera;
		}
		if (ImGui::Button("Top Stationary Camera", { 180, 30 }))
		{
			m_curCamera = m_topCamera;
		}

		//if (ImGui::DragFloat3("Camera: Position".c_str(), &lightPos[0], 0.1f))
		//{
		//	m_scene->SetPointLightPos(i, lightPos);
		//}
	}
	ImGui::SliderInt("Post Process Effect", &m_postProcessTarget, -1, 11);
	ImGui::End();

	if (m_planetsVisible)
	{
		ImGui::Begin("Solar System Settings");
		m_sun->ImGui();
		ImGui::End();
	}

	if (m_primitiveShapesVisible)
	{
		ImGui::Begin("Primitive Settings");
		ImGui::Checkbox("Toggle Square", &m_squareVisible);
		ImGui::Checkbox("Toggle Cylinder", &m_cylinderVisible);
		ImGui::Checkbox("Toggle Pyramid", &m_pyramidVisible);
		ImGui::Checkbox("Toggle Sphere", &m_sphereVisible);
		ImGui::End();
	}

	if (m_modelsVisible)
	{
		ImGui::Begin("Models Settings");
		for each (Instance* instance in m_scene->GetInstances())
		{
			instance->ImGui();
		}
		ImGui::End();
	}
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
	//m_gridTexture.bind(0);

	m_renderTarget.getTarget(0).bind(0);

	// Draw the quad using Mesh's draw
	m_quadMesh.Draw();
}

bool GraphicsApp::BunnyLoader()
{
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
	m_spearMesh.draw();
}

bool GraphicsApp::SpearLoader()
{
	if (m_spearMesh.load("./soulspear/soulspear.obj", true, true) == false)
	{
		printf("Soulspear Mesh Error!\n");
		return false;
	}

	m_spearTransform = {
		1, 0,  0,  0,
		0,  1, 0,  0,
		0,  0,  1, 0,
		0,  0,  0,  1
	};

	return true;
}

bool GraphicsApp::KamaDaggerLoader()
{
	if (m_kamadaggarMesh.load("./kamadagger/kamadagger.obj", true, true) == false)
	{
		printf("Kama Dagger Mesh Error!\n");
		return false;
	}

	m_kamadaggarTransform = {
		.005, 0, 0, 0,
		0, .005, 0, 0,
		0, 0, .005, 0,
		0, 0, 0, 1
	};

	return true;
}

bool GraphicsApp::ObjLoader(aie::OBJMesh& objMesh, glm::mat4& transform, float scale, 
	const char* filepath, bool flipTexture)
{
	if (objMesh.load(filepath, true, flipTexture) == false)
	{
		printf("Object Mesh Error!\n");
		return false;
	}

	transform = {
		scale, 0,     0,     0,
		0,     scale, 0,     0,
		0,     0,     scale, 0,
		0,     0,     0,     1
	};

	return true;
}
bool GraphicsApp::ObjLoader(aie::OBJMesh& objMesh, glm::mat4& transform, float scale, 
	const char* filepath, const char* filename, bool flipTexture)
{
	if (objMesh.load(filepath, true, flipTexture) == false)
	{
		std::string errorMessage = filename;
		errorMessage += " Mesh Error!\n";
		printf(errorMessage.c_str());
		return false;
	}

	transform = {
		scale, 0,     0,     0,
		0,     scale, 0,     0,
		0,     0,     scale, 0,
		0,     0,     0,     1
	};

	return true;
}

void GraphicsApp::ObjDraw(glm::mat4 pv, glm::mat4 transform, aie::OBJMesh* objMesh, aie::ShaderProgram* shader)
{
	// Bind the shader
	shader->bind();

	// Bind the camera position
	shader->bindUniform("CameraPosition",
		vec3(glm::inverse(m_viewMatrix)[3]));

	// Bind the directional light we defined
	shader->bindUniform("LightDirection", m_light.direction);
	shader->bindUniform("LightColor", m_light.color);
	shader->bindUniform("AmbientColor", m_ambientLight);

	// Bind the texture location
	shader->bindUniform("diffuseTexture", 0);

	// Bind the pvm using the one provided
	shader->bindUniform("ProjectionViewModel", pv * transform);

	// Bind the transform using the one provided
	shader->bindUniform("ModelMatrix", transform);

	// Draw the spear using the Mesh's draw
	objMesh->draw();
}

bool GraphicsApp::SquareLoader()
{
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

bool GraphicsApp::CylinderLoader(float radius, float height, int segments)
{
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

bool GraphicsApp::PyramidLoader(float width, float height)
{
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

bool GraphicsApp::SphereLoader(float segments, float rings, float radius)
{
	if (segments < 3)
	{
		printf("Number of segments of a sphere must be 3 or higher.\n");
		return false;
	}
	if (rings < 2)
	{
		printf("Number of rings of a sphere must be 2 or higher.\n");
		return false;
	}

	Mesh::Vertex* vertices = new Mesh::Vertex[(segments * (rings - 1)) + 2];
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

		float phi = ((PI) / rings) * r;
		for (int s = 0; s < segments; s++)
		{
			float theta = ((PI * 2) / segments) * s;
			vertices[verticesIndex].position = { radius * sin(phi) * cos(theta),
				radius * cos(phi), radius * sin(phi) * sin(theta), 1};
			verticesIndex++;
		}
	}

	//unsigned int indices[24] = { 2,1,0, 3,2,0, 4,3,0, 1,4,0 };
	unsigned int* indices = new unsigned int[((segments * 2) + ((rings - 2) * segments * 2)) * 3];
	int indicesIndex = 0;

	for (int r = 0; r < rings; r++)
	{
		for (int s = 0; s < segments; s++)
		{
			if (r == 0)
			{
				indices[indicesIndex] = s + 2 > segments ? 1 : s + 2;
				indices[indicesIndex + 1] = s + 1;
				indices[indicesIndex + 2] = 0;
				indicesIndex += 3;
				continue;
			}
			if (r == rings - 1)
			{
				indices[indicesIndex] = s + 1 + ((rings - 2) * segments);
				indices[indicesIndex + 1] = s + 2 > segments ? 1 + ((rings - 2) * segments) : s + 2 + ((rings - 2) * segments);
				indices[indicesIndex + 2] = (segments * (rings - 1)) + 1;
				indicesIndex += 3;
				continue;
			}
			
			indices[indicesIndex] = s + 2 > segments ? (r * segments) + 1 : s + 2 + (r * segments);
			indices[indicesIndex + 1] = s + 1 + (r * segments);
			indices[indicesIndex + 2] = s + 1 + ((r - 1) * segments);
			indicesIndex += 3;
			indices[indicesIndex] = s + 1 + ((r - 1) * segments);
			indices[indicesIndex + 1] = s + 2 > segments ? ((r - 1) * segments) + 1 : s + 2 + ((r - 1) * segments);
			indices[indicesIndex + 2] = s + 2 > segments ? (r * segments) + 1 : s + 2 + (r * segments);
			indicesIndex += 3;
		}
	}

	m_sphereMesh.Initailise((segments * (rings - 1)) + 2, vertices, 
		((segments * 2) + ((rings - 2) * segments * 2)) * 3, indices);

	// This is a 10 'unit' wide square
	m_sphereTransform = {
		1, 0,  0,  0,
		0,  1, 0,  0,
		0,  0,  1, 0,
		0,  0,  0,  1
	};

	return true;
}

void GraphicsApp::PrimitiveShapeDraw(glm::mat4 pvm, Mesh* mesh)
{
	// Bind the shader
	m_simpleShader.bind();

	// Bind the transform
	m_simpleShader.bindUniform("ProjectionViewModel", pvm);

	// Draw the quad using Mesh's draw
	mesh->Draw();
}
