#pragma once

#include "Gizmos.h"

#include <glm/glm.hpp>
#include <glm/ext.hpp>

using glm::vec2;
using glm::vec3;
using glm::mat4;

class BaseCamera {
public:
	BaseCamera();
	~BaseCamera() {};

	virtual void Update(float deltaTime);
	void Draw();
	
	// Getters
	vec3 GetPosition() 
		{ return m_position; }
	mat4 GetWorldTransform() 
		{ return m_worldTranform; }
	mat4 GetProjectionViewMatrix() 
		{ return m_projectionViewTransform; }
	mat4 GetProjectionMatrix()
		{ return m_projectionTransform; }
	mat4 GetViewMatrix() 
		{ return m_viewTransform; }
	float GetAspectRatio() 
		{ return m_aspectRatio; }

	// Setters
	void SetPosition(vec3 position) 
		{ m_position = position; }
	void SetAspectRatio(float width, float height) 
		{ m_aspectRatio = width / height; }
	void SetViewMatrix(vec3 from, vec3 to, vec3 up) 
		{ m_viewTransform = glm::lookAt(from, to, up); }
	void SetProjectionMatrix(float fieldOfView, float aspectRatio, float near, float far);
	void SetProjectionMatrix(float fieldOfView, float width, float height, float near, float far) 
		{ SetProjectionMatrix(fieldOfView, width / height, near, far); }

protected:
	mat4 m_projectionViewTransform;
	mat4 m_projectionTransform;
	mat4 m_worldTranform;
	mat4 m_viewTransform;

	vec3 m_position;

	float m_aspectRatio;
	float m_theta;
	float m_phi;

	vec2 m_lastMouse;

};

