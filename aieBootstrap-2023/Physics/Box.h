#pragma once
#include "Rigidbody.h"

class Box : public Rigidbody
{
public:
	Box(glm::vec2 position, glm::vec2 velocity, float orientation, float mass,
		glm::vec2 m_extents, float elasticity, glm::vec4 color);
	~Box();

	virtual void Draw(float alpha);

	bool CheckBoxCorners(const Box& box, glm::vec2& contact,
		int& numContacts, float& pen, glm::vec2& edgeNormal);

	// Getter
	glm::vec2 GetExtents() const { return m_extents; }
	float GetWidth() const { return m_extents.x * 2.f; }
	float GetHeight() const { return m_extents.y * 2.f; }

	// Setter
	void SetExtents(glm::vec2 extents) { m_extents = extents; }

protected:
	glm::vec2 m_extents;	// the halfedge lengths 

};

