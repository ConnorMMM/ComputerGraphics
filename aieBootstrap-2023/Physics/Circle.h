#pragma once

#include "Rigidbody.h"

class Circle : public Rigidbody
{
public:
	Circle(glm::vec2 position, glm::vec2 velocity, float mass, 
		float radius, float elasticity, glm::vec4 color);
	~Circle();

	virtual void Draw(float alpha);

	// Getter
	float GetRadius() { return m_radius; }

	// Setter
	void SetRadius(float radius) { m_radius = radius; }

protected:
	float m_radius;

};

