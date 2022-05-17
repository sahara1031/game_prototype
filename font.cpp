#include <iostream>
#include <stdarg.h>
#include "font.h"
#include "glut.h"
#include "glm/glm.hpp"

using namespace glm;
//using namespace std;

static vec2 position;
static float size = FONT_DEFAULT_SIZE;
static unsigned char color[3];

void fontBegin() {
	glPushMatrix(); //���ݐݒ肳��Ă���s���������Ȃ��悤�Ƀv�b�V��
	glPushAttrib(GL_ALL_ATTRIB_BITS); //GLbitfield mask ���ׂĂ̕`�摮�����v�b�V��

	glMatrixMode(GL_PROJECTION);//GLenum mode�@�s�񃂁[�h
	glLoadIdentity(); //void�@������

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport); //GLenum pname, GLint *params �E�B���h�E��x,y���W,����,�c����Ԃ�

	gluOrtho2D(
		0,//GLdouble left,
		viewport[2],//GLdouble right,
		viewport[3],//GLdouble bottom,
		0);//GLdouble top

	glMatrixMode(GL_MODELVIEW); //GLenum mode �s�񃂁[�h
	glLoadIdentity(); //void�@������

	glDisable(GL_DEPTH_TEST); //GLenum cap
	glDisable(GL_LIGHTING); //GLenum cap
	glDisable(GL_TEXTURE_2D); //GLenum cap
}

void fontEnd() {
	glPopMatrix();
	glPopAttrib(); //void
}

void fontSetPosition(float _x, float _y) {
	position = vec2(_x, _y);
}

void fontSetSize(float _size) {
	size = _size;
}

float fontGetSize(void) {
	return(size);
}

float fontGetWeightMin(void) {
	GLfloat weight[2];
	glGetFloatv(
		GL_LINE_WIDTH_RANGE,	//GLenum pname
		weight					//GLfloat *params
	);
	return weight[0];

}
float fontGetWeightMax(void) {
	GLfloat weight[2];
	glGetFloatv(
		GL_LINE_WIDTH_RANGE,	//GLenum pname
		weight					//GLfloat *params
	);
	return weight[1];
}

void fontSetWeight(float _weight) {

}

void fontSetColor(unsigned char _red, unsigned char _green, unsigned char _blue) {
	color[0] = _red;
	color[1] = _green;
	color[2] = _blue;
}

void fontDraw(const char *_format, ...) {
	va_list argList;
	va_start(argList, _format); //�ό����X�^�[�g
	char str[256];
	vsprintf_s(str, _format, argList);
	va_end(argList);

	glColor3ub(color[0], color[1], color[2]); //GLubyte red, GLubyte green, GLubyte blue unsigned �^�ŐF��`
	char* p = str;
	glPushMatrix();
	{
		glTranslatef(position.x,position.y + size, 0); //GLfloat x, GLfloat y, GLfloat z
		float s = size / FONT_DEFAULT_SIZE;
		glScalef(s, -s, s);
		for (; ( * p != '\0') && (*p != '\n'); p++)
			glutStrokeCharacter(GLUT_STROKE_ROMAN, *p);
	}
	glPopMatrix();

	if (*p == '\n') {
		glTranslatef(0,size + 5, 0);
		fontDraw(++p);
	}
	//std::cout << str << std::endl;
}