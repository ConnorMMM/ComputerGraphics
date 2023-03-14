#include "Circle.h"

#include <Gizmos.h>

Circle::Circle(glm::vec2 position, glm::vec2 velocity, float mass, float radius,
    float elasticity, glm::vec4 color) : 
    Rigidbody(CIRCLE, position, velocity, 0, mass, elasticity, color)
{
    m_radius = radius;

    m_moment = 0.5f * mass * radius * radius;
}

Circle::~Circle()
{
}

void Circle::Draw(float alpha)
{
    CalculateSmoothedPosition(alpha);

    if (!m_isHidden)
    {
        aie::Gizmos::add2DCircle(m_smoothedPosition, m_radius, 12, m_color);

        aie::Gizmos::add2DLine(m_smoothedPosition, m_smoothedPosition + m_smoothedLocalX * m_radius, glm::vec4(1, 1, 1, 1));
    }
}