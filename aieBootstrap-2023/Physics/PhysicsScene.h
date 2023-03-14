#pragma once

#include <glm/vec2.hpp>
#include <vector>

#define LINEAR_DRAG 0.3f
#define ANGULAR_DRAG 0.3f

#define MIN_LINEAR_THRESHOLD 0.2f
#define MIN_ANGULAR_THRESHOLD 0.01f

class PhysicsObject;
class Rigidbody;

class PhysicsScene
{
public:
	PhysicsScene();
	~PhysicsScene();

	void Update(float dt);
	void Draw();
	void debugScene();

	void AddActor(PhysicsObject* actor);
	void RemoveActor(PhysicsObject* actor);
	PhysicsObject* GetActor(int index) { return *(m_actors.begin() + index); }

	void CheckForCollision();
	static void ApplyContactForces(Rigidbody* body1, Rigidbody* body2, glm::vec2 norm, float pen);

	static bool Plane2Plane(PhysicsObject*, PhysicsObject*);
	static bool Plane2Circle(PhysicsObject*, PhysicsObject*);
	static bool Plane2Box(PhysicsObject*, PhysicsObject*);
	static bool Circle2Plane(PhysicsObject*, PhysicsObject*);
	static bool Circle2Circle(PhysicsObject*, PhysicsObject*);
	static bool Circle2Box(PhysicsObject*, PhysicsObject*);
	static bool Box2Plane(PhysicsObject*, PhysicsObject*);
	static bool Box2Circle(PhysicsObject*, PhysicsObject*);
	static bool Box2Box(PhysicsObject*, PhysicsObject*);

	float GetTotalEnergy();
	bool AllStationary();

	// Getters
	static glm::vec2 GetGravity() { return m_gravity; }
	float GetTimeStep() { return m_timeStep; }

	// Setters
	void SetGravity(const glm::vec2 gravity) { m_gravity = gravity; }
	void SetTimeStep(const float timeStep) { m_timeStep = timeStep; }

protected:
	static glm::vec2 m_gravity;
	float m_timeStep;
	std::vector<PhysicsObject*> m_actors;
};