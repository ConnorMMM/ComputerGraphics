#include "Rigidbody.h"
#include "PhysicsScene.h"

Rigidbody::Rigidbody(ShapeType shapeID, glm::vec2 position, glm::vec2 velocity, 
	float orientation, float mass, float elasticity, glm::vec4 color) : 
	PhysicsObject(shapeID, elasticity, color)
{
	m_position = position;
	m_velocity = velocity;
	m_orientation = orientation;
	m_mass = mass;
	m_angularVelocity = 0;

	m_linearDrag = LINEAR_DRAG;
	m_angularDrag = ANGULAR_DRAG;

	m_isKinematic = false;
	m_isTrigger = false;
	m_isHidden = false;
}

Rigidbody::~Rigidbody()
{
}

void Rigidbody::FixedUpdate(glm::vec2 gravity, float timeStep)
{
	m_lastPosition = m_position;
	m_lastOrientation = m_orientation;

	CalculateAxes();

	// trigger checks
	if (m_isTrigger)
	{
		// check every object that is inside us and called triggerEnter on
		// if they haven’t registered inside us this frame, they must have exited
		// so remove them from our list and call triggerExit
		for (auto it = m_objectsInside.begin(); it != m_objectsInside.end(); it++)
		{
			if (std::find(m_objectsInsideThisFrame.begin(), m_objectsInsideThisFrame.end(), *it) == m_objectsInsideThisFrame.end())
			{
				if (triggerExit)
					triggerExit(*it);
				it = m_objectsInside.erase(it);
				if (it == m_objectsInside.end())
					break;
			}
		}
	}

	// clear this list now for next frame
	m_objectsInsideThisFrame.clear();

	if (m_isKinematic)
	{
		m_velocity = glm::vec2(0);
		m_angularVelocity = 0;
		return;
	}

	m_position += m_velocity * timeStep;
	ApplyForce(gravity * m_mass * timeStep, glm::vec2(0));

	m_orientation += m_angularVelocity * timeStep;


	m_velocity -= m_velocity * m_linearDrag * timeStep;
	m_angularVelocity -= m_angularVelocity * m_angularDrag * timeStep;

	if (length(m_velocity) < MIN_LINEAR_THRESHOLD)
	{
		m_velocity = glm::vec2(0, 0);
	}
	if (abs(m_angularVelocity) < MIN_ANGULAR_THRESHOLD) {
		m_angularVelocity = 0;
	}
}

/// <summary>
/// Applies a given force, calculating the velocity and angular velocity.
/// </summary>
/// <param name="force">: The direction and magnitude of the force applied </param>
/// <param name="pos">: The local position that the force is applied to </param>
void Rigidbody::ApplyForce(glm::vec2 force, glm::vec2 pos)
{
	m_velocity += force / GetMass();
	m_angularVelocity += (force.y * pos.x - force.x * pos.y) / GetMoment();
}


void Rigidbody::ResolveCollision(Rigidbody* actor2, glm::vec2 contact,
	glm::vec2* collisionNormal, float pen)
{
	// register that these two objects have overlapped this frame
	m_objectsInsideThisFrame.push_back(actor2);
	actor2->m_objectsInsideThisFrame.push_back(this);

	// find the vector between their centres, or use the provided direction
	// of force, and make sure it's normalised
	glm::vec2 normal = glm::normalize(collisionNormal ? *collisionNormal :
		actor2->GetPosition() - GetPosition());
	// get the vector perpendicular to the collision normal
	glm::vec2 perp(normal.y, -normal.x);

	// determine the total velocity of the contact points for the two objects, 
	// for both linear and rotational 		

	// 'r' is the radius from axis to application of force
	float r1 = glm::dot(contact - GetPosition(), -perp);
	float r2 = glm::dot(contact - actor2->GetPosition(), perp);
	// velocity of the contact point on this object 
	float v1 = glm::dot(GetVelocity(), normal) - r1 * GetAngularVelocity();
	// velocity of contact point on actor2
	float v2 = glm::dot(actor2->GetVelocity(), normal) +
		r2 * actor2->GetAngularVelocity();
	if (v1 > v2) // they're moving closer
	{
		// calculate the effective mass at contact point for each object
		// ie how much the contact point will move due to the force applied.
		float mass1 = 1.0f / (1.0f / GetMass() + (r1 * r1) / GetMoment());
		float mass2 = 1.0f / (1.0f / actor2->GetMass() + (r2 * r2) / actor2->GetMoment());

		float elasticity = (GetElasticity() + actor2->GetElasticity()) / 2.0f;

		if (!m_isTrigger && !actor2->IsTrigger())
		{
			glm::vec2 force = (1.0f + elasticity) * mass1 * mass2 /
				(mass1 + mass2) * (v1 - v2) * normal;

			//apply equal and opposite forces
			ApplyForce(-force, contact - GetPosition());
			actor2->ApplyForce(force, contact - actor2->GetPosition());

			if (collisionCallback)
				collisionCallback(actor2);
			if (actor2->collisionCallback)
				actor2->collisionCallback(this);
		}
		else
		{
			TriggerEnter(actor2);
			actor2->TriggerEnter(this);
		}

		if (pen > 0)
			PhysicsScene::ApplyContactForces(this, actor2, normal, pen);
	}
}

void Rigidbody::CalculateSmoothedPosition(float alpha)
{
	m_smoothedPosition = alpha * m_position + (1 - alpha) * m_lastPosition;

	float smoothedOrientation = alpha * m_orientation
		+ (1 - alpha) * m_lastOrientation;

	float sn = sinf(smoothedOrientation);
	float cs = cosf(smoothedOrientation);

	m_smoothedLocalX = glm::vec2( cs, sn);
	m_smoothedLocalY = glm::vec2(-sn, cs);
}

void Rigidbody::CalculateAxes()
{
	float sn = sinf(m_orientation);
	float cs = cosf(m_orientation);
	m_localX = glm::vec2(cs, sn);
	m_localY = glm::vec2(-sn, cs);
}

glm::vec2 Rigidbody::ToWorld(glm::vec2 contact, float alpha)
{
	return m_position + m_localX * contact.x + m_localY * contact.y;
}

glm::vec2 Rigidbody::ToWorldSmoothed(glm::vec2 localPos)
{
	return m_smoothedPosition + m_smoothedLocalX * localPos.x + m_smoothedLocalY * localPos.y;
}

float Rigidbody::GetKineticEnergy()
{
	return .5f * (m_mass * glm::dot(m_velocity, m_velocity) + m_moment * m_angularVelocity * m_angularVelocity);
}

float Rigidbody::GetPotentialEnergy()
{
	return -GetMass() * glm::dot(PhysicsScene::GetGravity(), GetPosition());
}

void Rigidbody::TriggerEnter(PhysicsObject* actor2)
{
	if (m_isTrigger && std::find(m_objectsInside.begin(), m_objectsInside.end(), actor2) == m_objectsInside.end())
	{
		m_objectsInside.push_back(actor2);
		if (triggerEnter)
			triggerEnter(actor2);
	}
}