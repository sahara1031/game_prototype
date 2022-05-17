#pragma once
#include "glm/glm.hpp"

using namespace glm;

struct Ball
{
	vec2 m_position;
	vec2 m_speed;
	vec2 m_last_position;

	void update(void);
	void draw(void);
};