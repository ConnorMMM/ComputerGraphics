#pragma once

#include "BaseCamera.h"

class FlyCamera : public BaseCamera {
public:
	FlyCamera();
	~FlyCamera() {};

	virtual void Update(float deltaTime);

	// Getter
	float GetSpeed() 
		{ return m_speed; }

	// Setters
	void SetSpeed(float speed) 
		{ m_speed = speed; }
	void SetTurnSpeed(float turnSpeed)
		{ m_turnSpeed = turnSpeed; }

protected:
	float m_speed;
	float m_minSpeed;
	float m_maxSpeed;

	float m_turnSpeed;

	vec3 m_up;

};

