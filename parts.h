#pragma once
#pragma once

#include "glm/glm.hpp"

using namespace glm;

struct Ball {
	vec2 m_position;
	vec2 m_size;

	Ball(vec2 const& _position, vec2 const& _size);
	void Draw();
	bool intersect(vec2 const& _point);
	bool intersect(Ball const& _rect);
};

struct Squea
{
	vec2 m_position;
	vec2 m_size;

	Squea(vec2 const& _position, vec2 const& _size);
	void Draw();
	bool intersect(vec2 const& _point);
	bool intersect(Ball const& _rect);
};
