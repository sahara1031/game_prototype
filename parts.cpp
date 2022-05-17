#include "Rect.h"
#include "glut.h"
#include "glm/glm.hpp"
#include "parts.h"

using namespace glm;

Ball::Ball(vec2 const& _position, vec2 const& _size)
	:m_position(_position)
	, m_size(_size)
{}

void Ball::Draw() {
	glPushMatrix();
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	
	double rad = m_size.r;

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i <= 360; i+= 30) {
		glVertex2d(
			m_position.x + m_size.x / 2 + rad * cos(2 * 3.14 * i / 360),
			m_position.x + m_size.x / 2 + rad * cos(2 * 3.14 * i / 360)
		);
	}
	glEnd();

	glPopAttrib();
	glPushMatrix();
}

bool Ball::intersect(vec2 const& _point) {
	return true;
}

bool Ball::intersect(Ball const& _rect) {
	return true;
}