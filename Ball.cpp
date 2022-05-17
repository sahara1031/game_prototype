#include "Ball.h"
#include "glut.h"
//#include "glm/glm.hpp"

void Ball::update() {
	m_last_position = m_position;
	m_position += m_speed;
}

void Ball::draw() {
	glPushMatrix();
	{
		glTranslatef(m_position.x, m_position.y, 0);
		glScalef(16, 16, 0);
		glutSolidSphere(1, 16, 16); //GLdouble radius, GLint slices, GLint stacks
	}
	glPopMatrix();
}