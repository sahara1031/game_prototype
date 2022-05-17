#include <iostream>
#include <vector>
#include "glut.h"
#include "glm/glm.hpp"

using namespace glm;

#define BUFFER_MAX 256

#define WINDOW_WIDTH 720
#define WINDOW_HEIGHT 720

#define COURSE_WIDTH 155
#define COURSE_HEIGHT 17

#define PART_WIDTH (WINDOW_WIDTH / 16)
#define PART_HEIGHT (WINDOW_HEIGHT /16)

#define PLAYER_WIDTH PART_WIDTH
#define PLAYER_HEIGHT PART_HEIGHT

enum {
	TEX_UNITY_CHAN,
	TEX_BLOCK,
	TEX_MAX
};

enum {
	ANIMATION_STAND,
	ANIMATION_DAMAGE,
	ANIMATION_RUN,
	ANIMATION_JUMP,
	ANIMATION_MAX
};

typedef struct {
	BITMAPFILEHEADER bf;
	BITMAPINFOHEADER bi;
	GLuint texture;
	ivec2 rect;
}TEX;

typedef struct {
	int count;
	std::vector<int> frames;
}ANIMATION;

typedef struct {
	int animation;
	int frame;
	glm::vec2 position;
	glm::vec2 velocity;
	bool isMirror;
	bool isGround;
}PLAYER;

vec2 mouse_position;

const char course[COURSE_HEIGHT][COURSE_WIDTH] = {
"                                                                                                                                                         ",
"                                                                                                                                                         ",
"                                                                                                                                                         ",
"                                                                                    sssss                                                                ",
"             sssssss                                                                                                                                     ",
"             sssssss                                                                                                                                     ",
"                                                                    sssss                                                                                ",
"                                      sssss                                                                                                              ",
"                          ggg                       ssssss    ssss                  ssss                                                                 ",
"                                  ssssss                                                                                                                 ",
"                                                                                                                                                         ",
"                                          sssss                  ssssss          ssssss                   mmmmmmmm                                       ",
"                     ssssss                                                                             mmmmmmmmmmmmmmmm                                 ",
"                                                                                                                                                         ",
"                              mmm                                                                                                                        ",
"ssssssssssssssssssssss     ssssssssssssssssss     sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss",
"ssssssssssssssssssssss     ssssssssssssssssss     sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss"
};

bool keys[BUFFER_MAX];

ANIMATION animations[ANIMATION_MAX]{
	{4,{0,1,2,1}},//ANIMATION_STAND
	{2,{3,41}},//ANIMATION_DAMAGE
	{8,{7,8,9,10,14,15,16,17}},//ANIMATION_RUN
	{12,{21,22,23,24,25,26,27,26,25,24,23,22}}//ANIMATION_JUMP
};

TEX tex[TEX_MAX];
PLAYER player;

void TexFromFile(const char* _fileName,int _tex,ivec2 _rect) {
	tex[_tex].rect = _rect;
	FILE* stream;
	fopen_s(&stream, _fileName, "rb");
	if (!stream) {
		printf("%s open filed\n", _fileName);
	}
	printf("%s is opend\n", _fileName);

	fread(&tex[_tex].bf, sizeof BITMAPFILEHEADER, 1, stream);
	printf("bfsize:%d\n", tex[_tex].bf.bfSize);
	fread(&tex[_tex].bi, sizeof BITMAPINFOHEADER, 1, stream);
	printf("%d * %d \n", tex[_tex].bi.biWidth,tex[_tex].bi.biHeight);

	typedef struct {
		unsigned char b, g, r;
	}BGR;

	BGR* bits = (BGR*)malloc(sizeof(BGR) * tex[_tex].bi.biWidth * tex[_tex].bi.biHeight);
	fread(bits, sizeof BGR, tex[_tex].bi.biWidth* tex[_tex].bi.biHeight, stream);

	typedef struct {
		unsigned char r, g, b, a;
	}RGBA;

	RGBA* pixels = (RGBA*)malloc(sizeof(RGBA) * tex[_tex].bi.biWidth * tex[_tex].bi.biHeight);
	for (int i = 0; i < tex[_tex].bi.biWidth * tex[_tex].bi.biHeight; i++) {
		pixels[i].r = bits[i].r;
		pixels[i].g = bits[i].g;
		pixels[i].b = bits[i].b;
		pixels[i].a = ((bits[i].r == 0x00) && (bits[i].g == 0xff) && (bits[i].b == 0x00)) ? 0x00 : 0xff;
	}
	free(bits);
	for (int y = 0; y < tex[_tex].bi.biHeight / 2; y++) {
		for (int x = 0; x < tex[_tex].bi.biWidth; x++) {
			RGBA temp = pixels[y * tex[_tex].bi.biWidth + x];
			pixels[y * tex[_tex].bi.biWidth + x]
				= pixels[(tex[_tex].bi.biHeight - 1 - y) * tex[_tex].bi.biWidth + x];
			pixels[(tex[_tex].bi.biHeight - 1 - y) * tex[_tex].bi.biWidth + x] = temp;
		}
	}
	{

	}
	glGenTextures(1, &tex[_tex].texture);
	glBindTexture(GL_TEXTURE_2D,tex[_tex].texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGBA,
		tex[_tex].bi.biWidth,
		tex[_tex].bi.biHeight,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		pixels
	);

	free(pixels);

	//fread(bits, sizeof RGBA, tex[_tex].bi.biWidth* tex[_tex].bi.biHeight, stream);

	fclose(stream);
	printf("%s is closed\n",_fileName);
}

void Init(void) {
	TexFromFile("BMP/HERO.bmp",TEX_UNITY_CHAN,{64,64});
	TexFromFile("BMP/Block.bmp", TEX_BLOCK, { 24,24 });
	player.animation = ANIMATION_RUN;
	player.frame = 0;
	player.position = {WINDOW_WIDTH/2,PART_HEIGHT * 10};
	player.isMirror = false;
	player.isGround = true;
}

void DrawSprite(int _tex,float _width, float _height,int _index,glm::vec2 _position,bool _isMirror = false) {
	glBindTexture(GL_TEXTURE_2D, tex[_tex].texture);
	glEnable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glColor4ub(0xff, 0xff, 0xff, 0xff);

	glPushMatrix();
	{
		glTranslatef(_position.x + _width * _isMirror, _position.y,0);

		if (_isMirror) {
			glScalef(-1, 1, 0);
		}
		glBegin(GL_QUADS);
		{
			int w = tex[_tex].bi.biWidth;
			int h = tex[_tex].bi.biHeight;
			int tw = tex[_tex].rect.x;
			int th = tex[_tex].rect.y;
			int columnMax = w / tw;
			int rowMax = h / th;
			int tx = tw * (_index % columnMax);
			int ty = th * (_index / columnMax);
			glTexCoord2f(tx / (float)w, ty / (float)h);
			glVertex2f(0, 0);
			glTexCoord2f(tx / (float)w, (ty + th) / (float)h);
			glVertex2f(0, _height);
			glTexCoord2f((tx + tw) / (float)w, (ty + th) / (float)h);
			glVertex2f(_width, _height);
			glTexCoord2f((tx + tw) / (float)w, ty / (float)h);
			glVertex2f(_width, 0);
		}
		glEnd();
	}
	glPopMatrix();
}

void DrawAttack() {

}

void DrawPart(glm::vec2 _position, char _c) {
	int x = _position.x / PART_WIDTH;
	int y = _position.y / PART_HEIGHT;
	if ((x < 0) || (x >= COURSE_WIDTH) || (y < 0) || (y >= COURSE_HEIGHT))
		return;

	int index = -1;
	switch (_c) {
	case 's':
		index = 1;
		break;
	case 'g':
		index = 0;
		break;
	case 'o':
		index = 2;
		break;
	case 'm':
		index = 3;
		break;
	}
	if (index >= 0) {
		DrawSprite(TEX_BLOCK, PART_WIDTH, PART_HEIGHT, index, _position);
	}
	else {
		switch (_c) {
		case' ':
			glColor3ub(0x50, 0x80, 0x80);
			break;
		case'c':
			glColor3ub(0x00, 0x00, 0x00);
			break;
		case't':
			glColor3ub(0x98, 0xe0, 0x00);
			break;
		case'm':
			glColor3ub(0x48, 0xa8, 0x10);
			break;
		case'z':
			glColor3ub(0x00, 0x60, 0xd4);
			break;
		default:
			glColor3ub(0x00, 0x00, 0x00);
		}


		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
		glPushMatrix();
		{
			glTranslatef(_position.x, _position.y, 0);
			glBegin(GL_QUADS);
			{
				glVertex2f(0, 0);
				glVertex2f(0, PART_HEIGHT);
				glVertex2f(PART_WIDTH, PART_HEIGHT);
				glVertex2f(PART_WIDTH, 0);
			}
			glEnd();
		}
		glPopMatrix();
	}
}

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
	glMatrixMode(GL_MODELVIEW);
	//static int i = 0; i++;
	glLoadIdentity();

	float scrollX = player.position.x - WINDOW_WIDTH / 2 + PLAYER_WIDTH / 2;


	for (int y = 0; y < COURSE_HEIGHT; y++) {
		for (int x = 0; x < COURSE_WIDTH; x++) {
			DrawPart({ x * PART_WIDTH - scrollX,y * PART_HEIGHT }, course[y][x]);
		}
	}

	DrawSprite(
		TEX_UNITY_CHAN, 
		64 * PLAYER_WIDTH / 40.0f,
		64 * PLAYER_HEIGHT / 40.0f, 
		animations[player.animation].frames[
			(player.frame/8) % (animations[player.animation].count)
		],
		player.position + glm::vec2(-scrollX,0) - glm::vec2(12 * PLAYER_WIDTH / 40.0f,16 * PLAYER_HEIGHT / 40.0f),
				player.isMirror
	);
	//glutWireTeapot(100);
	glutSwapBuffers();
}

void Idle(void) {
	glutPostRedisplay();
}

bool IntersectField(glm::vec2 _position) {
	int x = (int)_position.x / PART_WIDTH;
	int y = (int)_position.y / PART_HEIGHT;
	if ((x < 0) || (x >= COURSE_WIDTH) || (y < 0) || (y >= COURSE_HEIGHT)) {
		return false;
	}
	switch (course[y][x]) {
	case 's':
	case 'p':
	case 'b':
	case 'q':
	case 'f':
		return true;
	}
	return false;
}


void Timer(int value) {
	vec2 acceleration = { 0.2f,0.5f};

	std::cout << acceleration.y << std::endl;
	if (keys['a'] ) {
		player.velocity.x -= acceleration.x;
		player.isMirror = true;
	}
	if (keys['d'] ) {
		player.velocity.x += acceleration.x;
		player.isMirror = false;
	}
	if (keys['g'] || player.isGround == true) {
		player.velocity.y -= 2.0f;//acceleration.y;
		player.isGround = false;
	}

	if ((keys['a']) || (keys['d'])) {
		player.animation = ANIMATION_RUN;
	}
	else {
		player.animation = ANIMATION_STAND;
	}


	player.velocity.x *= 0.95f;
	player.velocity.y += 0.7f;
	player.position += player.velocity;

	glm::vec2 playerBottom = player.position + glm::vec2(PLAYER_WIDTH / 2.0f, PLAYER_HEIGHT);
	if (IntersectField(playerBottom)) {
		player.velocity.y = 0;
		player.position.y = (float)((int)player.position.y / PART_HEIGHT * PART_HEIGHT);
		player.isGround = true;
	}

	glm::vec2 playerRight = player.position + glm::vec2(PLAYER_WIDTH , PLAYER_HEIGHT / 2.0f);
	if (IntersectField(playerRight)) {
		player.velocity.x = 0;
		player.position.x = (float)((int)player.position.x / PART_WIDTH * PART_WIDTH);
	}

	glm::vec2 playerLEFT = player.position + glm::vec2(0, PLAYER_HEIGHT / 2.0f);
	if (IntersectField(playerLEFT)) {
		player.velocity.x = 0;
		player.position.x = (float)((int)player.position.x / PART_WIDTH * PART_WIDTH);
	}

	glutPostRedisplay();
	player.frame++;
	glutTimerFunc(1000 / 60, Timer, 0);
}

void keyboard(unsigned char key, int x, int y) {//x,y = マウスの座標


	if (keys['a']) {
		//player.isMirror = true;
	}
	if (keys['d']) {
		//player.isMirror = false;
	}
	std::cout << "keyboard: " << key << "  " << std::hex << int(key) << std::endl;
	keys[key] = true;	

	mouse_position = { x,y };
}

void keyboardUP(unsigned char key, int x, int y) {
	keys[key] = false;	
	player.velocity.x = 0;
}


int main(int argc,char *argv[]) {
	glutInit(
		&argc,
		argv);
	glutInitDisplayMode(GL_DOUBLE); //unsigned int mode
	glutInitWindowPosition(640, 0);	//int x, y
	glutInitWindowSize(WINDOW_WIDTH,WINDOW_HEIGHT);	//int width, height
	glutCreateWindow("GAME");// const char *title
	glutDisplayFunc(display); //void (GLUTCALLBACK *func)(void)
	glutTimerFunc(0, Timer, 0);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboardUP);
	//glutIdleFunc(Idle);
	Init();
	glutMainLoop(); // (void)

}
