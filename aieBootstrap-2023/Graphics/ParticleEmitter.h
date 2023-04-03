#pragma once
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/ext.hpp>
#include <gl_core_4_4.h>

struct Particle {
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec4 color;

	float size;
	float lifespan;
	float lifetime;
};

struct ParticleVertex {
	glm::vec4 position;
	glm::vec4 color;
};

class ParticleEmitter
{
public:
	ParticleEmitter();
	ParticleEmitter(unsigned int _maxParticles, unsigned int _emitRate,
		float _lifespanMin, float _lifespanMax, float _velocityMin,
		float _velocityMax, float _startSize, float _endSize,
		const glm::vec4& _startColor, const glm::vec4& _endColor);
	virtual ~ParticleEmitter();

	void Initialise(unsigned int _maxParticles, unsigned int _emitRate,
		float _lifespanMin, float _lifespanMax, float _velocityMin,
		float _velocityMax, float _startSize, float _endSize,
		const glm::vec4& _startColor, const glm::vec4& _endColor);

	void Emit();
	void Update(float _dt, const glm::mat4& _cameraTransform);
	void Draw();

	// Setters
	void SetPosition(glm::vec3 position) { m_position = position; }
	void SetColor(glm::vec4 color);

protected:
	Particle* m_particles;
	unsigned int m_firstDead;
	unsigned int m_maxParticles;

	unsigned int m_vao, m_vbo, m_ibo;
	ParticleVertex* m_vertexData;

	glm::vec3 m_position;
	
	float m_emitTimer;
	float m_emitRate;

	float m_lifespanMin;
	float m_lifespanMax;

	float m_velocityMin;
	float m_velocityMax;

	float m_startSize;
	float m_endSize;

	glm::vec4 m_startColor;
	glm::vec4 m_endColor;

	glm::vec3 m_gravity = { 0, 0, 0 };
	bool m_hasGravity = false;

};