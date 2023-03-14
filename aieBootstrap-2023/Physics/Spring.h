#pragma once

#include "PhysicsObject.h"
#include "Rigidbody.h"
#include "glm/glm.hpp"

class Spring : public PhysicsObject
{
public:
	Spring(Rigidbody* body1, Rigidbody* body2, float springCoefficient, float damping, float restLength = 0, 
		glm::vec2 contact1 = glm::vec2(0), glm::vec2 contact2 = glm::vec2(0));
	~Spring();

	virtual void FixedUpdate(glm::vec2 gravity, float timeStep);
	virtual void Draw(float alpha);
	virtual void ResetPosition() {}

	virtual float GetKineticEnergy() { return 0; }
	virtual float GetEnergy() { return 0; }

	// Getters
	glm::vec2 GetContact1(float alpha) 
		{ return m_body1 ? m_body1->ToWorldSmoothed(m_contact1) : m_contact1; }
	glm::vec2 GetContact2(float alpha) 
		{ return m_body2 ? m_body2->ToWorldSmoothed(m_contact2) : m_contact2; }

protected:
	Rigidbody* m_body1;
	Rigidbody* m_body2;

	glm::vec2 m_contact1;
	glm::vec2 m_contact2;

	float m_damping;
	float m_restLength;
	float m_springCoefficient; // the restoring force
};

