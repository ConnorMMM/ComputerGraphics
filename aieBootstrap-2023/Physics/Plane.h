#pragma once

#include "PhysicsObject.h"
#include "Rigidbody.h"

class Plane : public PhysicsObject
{
public:
    Plane(glm::vec2 normal, float distance, float elasticity, glm::vec4 color);
    ~Plane();

    virtual void FixedUpdate(glm::vec2 gravity, float timeStep);
    virtual void Draw(float alpha);
    virtual void ResetPosition();

    void ResolveCollision(Rigidbody* actor2, glm::vec2 contact);

    virtual float GetKineticEnergy() { return 0; }
    virtual float GetEnergy() { return 0; }

    // Getters
    glm::vec2 GetNormal() { return m_normal; }
    float GetDistance() { return m_distanceToOrigin; }

protected:
    glm::vec2 m_normal;
    float m_distanceToOrigin;
};

