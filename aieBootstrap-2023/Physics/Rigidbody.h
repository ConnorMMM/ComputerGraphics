#pragma once

#include "PhysicsObject.h"

#include <glm/glm.hpp>
#include <functional>
#include <list>

class Rigidbody : public PhysicsObject
{
public:
	Rigidbody(ShapeType shapeID, glm::vec2 position, glm::vec2 velocity, 
		float orientation, float mass, float elasticity, glm::vec4 color);
	~Rigidbody();

	virtual void FixedUpdate(glm::vec2 gravity, float timeStep);
	void ApplyForce(glm::vec2 force, glm::vec2 pos);
	
	void ResolveCollision(Rigidbody* actor2, glm::vec2 contact, glm::vec2* collisionNormal = nullptr, float pen = 0);
	
	void CalculateSmoothedPosition(float alpha);

	void CalculateAxes();

	glm::vec2 ToWorld(glm::vec2 contact, float alpha);
	glm::vec2 ToWorldSmoothed(glm::vec2 localPos);

	virtual float GetKineticEnergy();
	float GetPotentialEnergy();
	virtual float GetEnergy() { return GetKineticEnergy() + GetPotentialEnergy(); }

	void TriggerEnter(PhysicsObject* actor2);

	// Getters
	glm::vec2 GetPosition()	const { return m_position; }
	glm::vec2 GetLastPosition()	{ return m_lastPosition; }
	glm::vec2 GetVelocity()	{ return m_velocity; }
	float GetMass()	{ return m_isKinematic ? INT_MAX : m_mass; }

	float GetOrientation() { return m_orientation; }
	float GetLastOrientation() { return m_lastOrientation; }
	float GetAngularVelocity() { return m_angularVelocity; }
	float GetMoment() { return m_isKinematic ? INT_MAX : m_moment; }

	glm::vec2 GetSmoothedPosition() { return m_smoothedPosition; }
	glm::vec2 GetSmoothedLocalX() { return m_smoothedLocalX; }
	glm::vec2 GetSmoothedLocalY() { return m_smoothedLocalY; }

	glm::vec2 GetLocalX() { return m_localX; }
	glm::vec2 GetLocalY() { return m_localY; }

	float GetLinearDrag() { return m_linearDrag; }
	float GetAngularDrag() { return m_angularDrag; }

	bool IsKinematic() { return m_isKinematic; }
	bool IsTrigger() { return m_isTrigger; }
	bool IsHidden() { return m_isHidden; }

	// Setters
	void SetPosition(glm::vec2 position) { m_position = position; }
	void SetVelocity(glm::vec2 velocity) { m_velocity = velocity; }
	void SetMass(float mass) { m_mass = mass; }

	void SetOrientation(float orientation) { m_orientation = orientation; }
	void SetAngularVelocity(float angularVelocity) { m_angularVelocity = angularVelocity; }
	void SetMoment(float moment) { m_moment = moment; }

	void SetLinearDrag(float linearDrag) { m_linearDrag = linearDrag; }
	void SetAngularDrag(float angularDrag) { m_angularDrag = angularDrag; }

	void SetKinematic(bool state) { m_isKinematic = state; }
	void SetTrigger(bool state) { m_isTrigger = state; }
	void SetHidden(bool state) { m_isHidden = state; }

	std::function<void(PhysicsObject*)> collisionCallback;

	std::function<void(PhysicsObject*)> triggerEnter;
	std::function<void(PhysicsObject*)> triggerExit;

protected:
	glm::vec2 m_position;
	glm::vec2 m_lastPosition;
	glm::vec2 m_velocity;
	float m_mass;

	float m_orientation; // 2D so we only need a single float to represent our orientation
	float m_lastOrientation;
	float m_angularVelocity;
	float m_moment;

	glm::vec2 m_smoothedPosition;
	glm::vec2 m_smoothedLocalX;
	glm::vec2 m_smoothedLocalY;

	// store the local x,y axes of the box based on its angle of rotation
	glm::vec2 m_localX;
	glm::vec2 m_localY;

	float m_linearDrag;
	float m_angularDrag;

	bool m_isKinematic;
	bool m_isTrigger;
	bool m_isHidden;

	std::list<PhysicsObject*> m_objectsInside;
	std::list<PhysicsObject*> m_objectsInsideThisFrame;
};