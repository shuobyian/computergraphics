#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>
#include <GL/glext.h>
#include <GL/freeglut.h>
#include <GL/GLAUX.H>
#include <Mmsystem.h>
#pragma comment(lib, "legacy_stdio_definitions.lib")
#pragma comment(lib, "GLAUX.LIB")
#pragma comment(lib, "winmm.lib")

#include <string.h>
#include <vector>
#include <random>
#include <ctime>
#include <cstdlib>

#include <fstream>
#include <Windows.h>
#include <math.h>
using namespace std;

#include "ObjParser.h"
#include "bmpfuncs.h"

#include "Digitalv.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define PI 3.14159265
#define OBJNUMBER 6
#define COLORNUMBER 9
#define snowNum 1000
#define particleNum 1000

void init(void);
void resize(int, int);

/* texture mapping */
void cubeTextureMapping();
void myTextureMapping();
void castleTextureMapping();
void skyboxTextureMapping();
void sphereTextureMapping();

/* draw */
void draw(void);

void draw_obj(ObjParser);
void draw_obj_texture(ObjParser);
ObjParser* lego;
ObjParser* igloo;
ObjParser* olaf;
ObjParser* snowball;
ObjParser* castle[6];
ObjParser* soldier;

void draw_lego(void);
void draw_olaf(void);
void draw_rec(void); // 임시 저장된 lego 불러오기
void draw_snow(void);
float br(float, float);
void draw_snow_init(void);
void draw_castle(void);
void draw_soldier1(void);
void draw_soldier2(void);
void draw_soldier3(void);

void draw_axis(void);
void draw_string(void*, const char*, float, float, float, float, float);
void draw_text(void);

void draw_textureCube(void);
void draw_skybox(void); // background
void draw_square(void);
void draw_sphere(void);

/* input utility */
void mouse(int, int, int, int);
void motion(int, int);
void mouseWheel(int but, int dir, int x, int y);
void keyboard(unsigned char, int, int);
void specialkeyboard(int, int, int);
void idle(void);

/* block save */
void save_legoblock(void); // lego block 하나하나 임시 저장
void save_lego(void); // 지금까지 저장한 lego 파일로 저장
void open_lego(void); // 저장된 파일 불러와 lego rendering

/* soldier game */
void move_soldier();
void collision();

/* 불꽃 놀이*/
void draw_particle1(void);
void draw_particle2(void);
void draw_particle3(void);
void draw_particle4(void);
void draw_particle5(void);

/* default */
void light0(void);
void light1(void);
void light2(void);
void light3(void);
void printInstruction(void);
void setDefault(void);
void calculate_eye(void);

void sub_menu_function(int);
void main_menu_function(int);

GLuint texName;	// make cube
GLuint texName2; // olaf
GLuint texName3[6]; // castle
GLuint g_nCubeTex; // skybox
GLuint texName4[3]; // bright, fire (sphere)

int rampartTex = 1;
int roofTex = 1;

/* quadric object 생성 */
GLUquadricObj* qobj = gluNewQuadric();
GLUquadricObj* qobj2 = gluNewQuadric();

double theta;
double phi;
int radius;
double upVector = cos(phi * PI / 180);
double eyex, eyey, eyez;

int mouse_lstate = 1; // 마우스 왼쪽 클릭 상태
int mouse_rstate = 1; // 마우스 오른쪽 클릭 상태
int mouse_lx = 0; // 시점 이동에 쓰일 마우스 왼쪽 클릭 상태로 이동시 좌표
int mouse_ly = 0;

int opening = 0;		// 시작 화면
int music = 1;			// 음악1 on / off
int music2 = 1;			// 음악2 on / off
int game_mode = 0;		// 게임 모드
int insert_mode = 1;	// 도형 및 색상 선택 모드
int insert_mode2 = 0;	// 위치 선택 모드
int snow_mode = 0;		// 눈 내리기 모드
int center_view = 0;	// 성 시작 시 카메라 중심 변화
double centerx, centery, centerz;
int shape = 0;			// 도형
int color = 0;			// 색깔
double alpha = 1.0f;	// 투명도
double legox, legoy, legoz; // 블럭 위치
double olafx, olafz, olafangle; // 올라프 위치
double angle;			// 각도
double scale;			// 크기

/* 눈 던지기 (포물선 운동) */
double gravity = 0.98;
double velocity = 1.0f;
double acc = 30;
double theta_ = 20;
double phi_ = 20;
double wind = 0;
double time1 = 0.0f;
double snowballx, snowbally, snowballz, snowballx0, snowbally0, snowballz0;

/* 불꽃놀이 */
int fire1 = 0, fire2 = 0, fire3 = 0, fire4 = 0, fire5 = 0;
int star = 0, stary = -30;
int particle1 = 0, particle2 = 0, particle3 = 0, particle4 = 0, particle5 = 0;
double firex, firez;
double firey1, firey2, firey3, firey4, firey5;
double pointx, pointy, pointz;

/* 게임 모드 */
double soldier1x, soldier1y, soldier1z;
double soldier2x, soldier2y, soldier2z;
double soldier3x, soldier3y, soldier3z;
double soldierangle1, soldierangle2, soldierangle3;
int lose = 0, win = 0;

typedef unsigned char uchar;

char objName[OBJNUMBER][255] = {
	"obj/onebyone.obj",
	"obj/onebytwo.obj",
	"obj/twobyfour.obj",
	"obj/cylinder.obj",
	"obj/nose.obj",
	"obj/arm.obj",
};
char objName2[255] = "obj/igloo2.obj";
char objName3[255] = "obj/olaf.obj";
char objName4[255] = "obj/soldier.obj";
char objCastle[6][255] = {
	"obj/castle/blue.obj",
	"obj/castle/brown.obj",
	"obj/castle/flags.obj",
	"obj/castle/rampart.obj",
	"obj/castle/roof.obj",
	"obj/castle/window.obj",
};

struct legoblock {
	int shape;
	int color;
	double legox;
	double legoy;
	double legoz;
	double angle;
	double scale;
};
vector<legoblock> saveObj;
vector<GLfloat*> saveMatrix;
GLfloat temp_Matrix[16];

float colorMatrix[9][3] = {
	{1.0f,0.0f,0.0f}, //red
	{0.0f,1.0f,0.0f}, //green
	{0.0f,0.0f,1.0f}, //blue
	{1.0f,1.0f,0.0f}, //yellow
	{1.0f,0.5f,0.0f}, //orange
	{0.5f,1.0f,0.5f}, //light green
	{1.0f,0.5f,1.0f}, //purple
	{0.0f,0.0f,0.0f}, //black
	{1.0f,1.0f,1.0f}  //white
};

struct Attrib_snow {
	float position[3];
	float velocity[3];
	float color[3];
	float energy;
	float size;
};

Attrib_snow snow[snowNum];

int main(int argc, char** argv) {
	srand((unsigned int)time(NULL));

	int submenu1, submenu2;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(400, 300);
	glutCreateWindow("My First GL Program");
	init();

	PlaySound(TEXT("theThirdFig.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP | SND_NODEFAULT);
	submenu1 = glutCreateMenu(sub_menu_function);
	glutAddMenuEntry("gray", 11);
	glutAddMenuEntry("black", 12);
	glutAddMenuEntry("orange", 13);

	submenu2 = glutCreateMenu(sub_menu_function);
	glutAddMenuEntry("brown", 21);
	glutAddMenuEntry("green", 22);
	glutAddMenuEntry("purple", 23);

	glutCreateMenu(main_menu_function);
	glutAddMenuEntry("Clear", 999);
	glutAddMenuEntry("Exit", 11);
	glutAddMenuEntry("Save", 1);
	glutAddMenuEntry("Open", 2);
	glutAddSubMenu("rampart", submenu1);
	glutAddSubMenu("roof", submenu2);

	glutAttachMenu(GLUT_RIGHT_BUTTON);

	/* quadric object 속성 설정 */
	gluQuadricDrawStyle(qobj, GLU_FILL);
	gluQuadricNormals(qobj, GLU_SMOOTH);

	gluQuadricDrawStyle(qobj2, GLU_FILL);
	gluQuadricNormals(qobj2, GLU_SMOOTH);

	glutDisplayFunc(draw);

	glutIdleFunc(idle);

	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutMouseWheelFunc(mouseWheel);

	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialkeyboard);

	glutMainLoop();

	return 0;
}

void init(void) {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);   //버퍼 초기값 설정(R,G,B,투명도)
	glMatrixMode(GL_PROJECTION);
	glClearDepth(1.0f);         //버퍼 Clearing시 이용할 값 등록
	glLoadIdentity();
	gluOrtho2D(0.0f, 500.0f, 0.0f, 500.0f);

	glColor3f(1.0f, 1.0f, 1.0f);

	glutReshapeFunc(resize);

	/* default */
	setDefault();

	/* light default */
	glEnable(GL_LIGHTING);

	/* 조작법 출력 */
	printInstruction();

	/* 눈 내리기 */
	draw_snow_init();

	/* 불꽃 놀이 */

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glEnable(GL_TEXTURE_2D);

	glFrontFace(GL_CCW);

	/* blending & antialiasing */
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glEnable(GL_DEPTH_TEST);
}
void resize(int width, int height) {
	glViewport(0, 0, width, height);
	printf("[resize] WIDTH : %d,  HEIGHT : %d\n", width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (float)width / (float)height, 1, 500);
	glMatrixMode(GL_MODELVIEW);
}
void light0() {
	glClearColor(0, 0, 0, 1.0f);

	GLfloat ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat diffuseLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat specularLight[] = { 0.5f, 0.5f, 0.5f, 1.0f };

	glShadeModel(GL_SMOOTH);

	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);

	/************* spot position setting *************/
	/*GLfloat spot_direction[] = { 10.0, 10.0, 10.0, 1.0 };
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 60.0);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spot_direction);
	glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 5);*/


	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	/* global light setting */
	//glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

	glEnable(GL_LIGHT0);
}

void light1(void) {
	glDisable(GL_LIGHT0);
	GLfloat ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat diffuseLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat specularLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat specularMaterial[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	glShadeModel(GL_SMOOTH);

	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glLightfv(GL_LIGHT1, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specularLight);

	glMaterialfv(GL_FRONT, GL_SPECULAR, specularMaterial);
	glMateriali(GL_FRONT, GL_SHININESS, 60);

	glEnable(GL_COLOR_MATERIAL);

	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	glEnable(GL_LIGHT1);
}
void light2(void) {
	GLfloat ambientLight[] = { 0.3f,0.3f,0.3f,1.0f };
	GLfloat diffuseLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat specularLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat specularMaterial[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	glShadeModel(GL_SMOOTH);

	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glLightfv(GL_LIGHT2, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specularLight);

	glMaterialfv(GL_FRONT, GL_SPECULAR, specularMaterial);
	glMateriali(GL_FRONT, GL_SHININESS, 60);

	glEnable(GL_COLOR_MATERIAL);
	//glColorMaterial(GL_FRONT, GL_DIFFUSE);
	//glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_LIGHT2);
}
void light3(void) {
	GLfloat ambientLight[] = { 0.0f,0.0f,0.0f,1.0f };
	GLfloat diffuseLight[] = { 1.0f, 1.0f,1.0f, 1.0f };

	glShadeModel(GL_SMOOTH);

	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glLightfv(GL_LIGHT3, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT3, GL_DIFFUSE, diffuseLight);

	glEnable(GL_COLOR_MATERIAL);
	//glColorMaterial(GL_FRONT, GL_DIFFUSE);
	//glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_LIGHT3);
}


void idle(void) {
	if (fire1) {
		firey1 += 7.0;
		if (firey1 > 28) {
			firey1 = 30;
			particle1 = 1;
			fire2 = 1;
		}
	}
	if (fire2) {
		firey2 += 7.0;
		if (firey2 > 14) {
			firey2 = 15;
			particle1 = 0;
			fire1 = 0;
			particle2 = 1;
			fire3 = 1;
		}
	}
	if (fire3) {
		firey3 += 7.0;
		if (firey3 > 28) {
			firey3 = 25;
			particle2 = 0;
			particle3 = 1;
			fire2 = 0;
			fire4 = 1;
		}
	}
	if (fire4) {
		firey4 += 7.0;
		if (firey4 > 35) {
			firey4 = 35;
			particle3 = 0;
			particle4 = 1;
			fire3 = 0;
			fire5 = 1;
		}
	}
	if (fire5) {
		firey5 += 7.0;
		if (firey5 > 28) {
			firey5 = 30;
			particle4 = 0;
			particle5 = 1;
			fire4 = 0;
			star = 1;
		}
	}
	if (star) {
		stary += 5.0f;
		if (stary > 30) {
			stary = 30;
			particle5 = 0;
			fire5 = 0;
		}
	}
	if (center_view) {
		centery -= 4;
		if (centery < 0.0) {
			centery = 0.0;
		}
	}
	glutPostRedisplay();
}
void mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON) {
		mouse_lstate = state;
	}
	if (button == GLUT_RIGHT_BUTTON) {
		mouse_rstate = state;
	}
}
void motion(int x, int y) {
	//if (opening == 1 && game_mode != 2) {
		if (mouse_lstate == 0) {
			if (mouse_lx > x) {
				theta += 3;
				if (theta >= 360) theta = 0.0;
			}
			else if (mouse_lx < x) {
				theta -= 3;
				if (theta <= -360) theta = 0.0;
			}
			if (mouse_ly < y) {
				phi += 3;
				if (phi >= 51.0) phi = 51.0;
			}
			else if (mouse_ly > y) {
				phi -= 3;
				if (phi < -3) phi = -3;
			}
		}
		mouse_lx = x;
		mouse_ly = y;
	//}
	glutPostRedisplay();
}
void mouseWheel(int but, int dir, int x, int y) {
	if (opening == 1 && game_mode != 2) {
		if (dir > 0) {
			if (radius > 2) radius--;
		}
		else {
			if (radius < 360) radius++;
		}
	}
	glutPostRedisplay();
}
void game_mode_keyboard(unsigned char key, int x, int y) {
	if (game_mode == 1) {
		if (key == 'l') { // 눈 on/off
			snow_mode = !snow_mode;
		}
		if (key == 'f') {
			sndPlaySoundA(TEXT("fire.wav"), SND_ASYNC | SND_NODEFAULT);
			fire1 = !fire1;
			firey1 = -30, firey2 = -30, firey3 = -30, firey4 = -30, firey5 = -30;  stary = -30;
			star = 0;
		}
	}
	else {
		if (key == 'm') {
			music2 = !music2;
			if (music2) PlaySound(TEXT("Gelimer_lab.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP | SND_NODEFAULT);
			else PlaySound(NULL, 0, 0);
		}
	}

}
void non_game_mode_keyboard(unsigned char key, int x, int y) {
	if (key == 'i') { // 도형 및 색깔 선택 모드
		insert_mode = !insert_mode;
		insert_mode2 = 0;
		legox = 0.0;
		legoy = 0.0;
		legoz = 0.0;
		angle = 0.0;
		scale = 1.0;
	}
	if (insert_mode) {
		if (key == 'x') {
			shape++;
			if (shape > OBJNUMBER - 1) shape = 0;
		}
		if (key == 'c') {
			color++;
			if (color > COLORNUMBER - 1) color = 0;
		}
	}
	if (insert_mode2) {
		if (key == 'w') {
			legoz -= 0.1;
			if (legoz < -30.0) legoz = -30.0;
		} // front
		if (key == 's') {
			legoz += 0.1;
			if (legoz > 30.0) legoz = 30.0;
		} // back
		if (key == 'a') {
			legox -= 0.1;
			if (legox < -30.0) legox = -30.0;
		} // left
		if (key == 'd') {
			legox += 0.1;
			if (legox > 30.0) legox = 30.0;
		} // right
		if (key == 'q') {
			legoy += 0.1;
			if (legoy > 14.0) legoy = 14.0;
		} // up
		if (key == 'e') {
			legoy -= 0.1;
			if (legoy < 0.0) legoy = 0.0;
		} // down
		if (key == '/') {
			angle += 90.0;
			if (angle >= 360) angle = 0;
			cout << angle << endl;
		}
		if (key == '.') {
			scale += 0.1;
		}
		if (key == ',') {
			scale -= 0.1;
			if (scale < 0.1) scale = 0.1;
		}
	}
	if (insert_mode) {
		if (key == 'o') { // 도형 및 색깔 선택 완료
			insert_mode = 0;
			insert_mode2 = 1;
		}
	}
	if (insert_mode2) {
		if (key == 'p') { // 블록 위치까지 최종 선택 완료
			save_legoblock();
		}
	}
	if (key == 'u') {
		if (!saveObj.empty())
			saveObj.pop_back();
	} // 원래대로
}
void keyboard(unsigned char key, int x, int y) {
	if (key == 13 && opening == 0) { // 게임 시작
		opening = 1;
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		/* TEXTURE MAPPING SET */
		cubeTextureMapping();
		myTextureMapping();
		castleTextureMapping();
		skyboxTextureMapping();
		sphereTextureMapping();
		gluQuadricTexture(qobj, GL_TRUE);
		gluQuadricTexture(qobj2, GL_TRUE);
	} //enter
	if (game_mode < 2) {
		if (key == 'm') {
			music = !music;
			if (music) PlaySound(TEXT("theThirdFig.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP | SND_NODEFAULT);
			else PlaySound(NULL, 0, 0);
		}
	}
	if (opening == 1) {
		if (key == 'g') {
			PlaySound(TEXT("theThirdFig.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP | SND_NODEFAULT);
			game_mode++;
			if (game_mode > 2) game_mode = 0;
			center_view = !center_view;
			if (game_mode == 1) {
				centery = 70;
				radius = 116.0f;
				theta = -30;
				phi = 3;
				calculate_eye();
			}
			else if (game_mode == 2) {
				PlaySound(TEXT("Gelimer_lab.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP | SND_NODEFAULT);
				radius = 116.0f;
				theta = -30;
				phi = 3;
				olafx = 10; olafz = 0;
				calculate_eye();
			}
			else { // 블럭 조작 모드
				radius = 25.0f;
				theta = 42;
				phi = 39;
				calculate_eye();
			}
		}
		if (game_mode > 0) {
			game_mode_keyboard(key, x, y);
		}
		else {
			non_game_mode_keyboard(key, x, y);
		}
	}
	glutPostRedisplay();
}
void specialkeyboard(int key, int x, int y) {
	if (game_mode != 0) {
		switch (key) {
		case GLUT_KEY_LEFT:
			olafz += 1.0f;
			olafangle = 270.0f;
			break;
		case GLUT_KEY_RIGHT:
			olafz -= 1.0f;
			olafangle = 90.0f;
			break;
		case GLUT_KEY_DOWN:
			olafx += 1.0f;
			olafangle = 360.0f;
			break;
		case GLUT_KEY_UP:
			olafx -= 1.0f;
			olafangle = 180.0f;
			break;
		case 5:
			printInstruction();
			//setDefault();
			break;
		default:
			printf("%d is pressed\n", key);
			break;
		}
		cout << olafx << " " << olafz << endl;
	}
	glutPostRedisplay();
}

void sub_menu_function(int option) {
	printf("Submenu %d has been slected\n", option);
	if (option == 11) {
		rampartTex = 1;
	}
	if (option == 12) {
		rampartTex = 2;
	}
	if (option == 13) {
		rampartTex = 3;
	}
	if (option == 21) {
		roofTex = 1;
	}
	if (option == 22) {
		roofTex = 2;
	}
	if (option == 23) {
		roofTex = 3;
	}
	castleTextureMapping();
	glutPostRedisplay();
}
void main_menu_function(int option) {
	printf("Main menu %d has been selected\n", option);
	if (option == 999) {
		setDefault();
	}
	if (option == 11) {
		exit(0);
	}
	if (option == 1) {
		save_lego();
	}
	if (option == 2) {
		open_lego();
	}
}
void printInstruction() {
	/* 조작법 console 출력 */
	printf("\n------------keyboard navigation------------\n");
	printf("방향키 : camera 위치\n");
	printf("F5 : 새로고침\n\n");
}

void setDefault() {
	legox = 0.0;
	legoy = 0.0;
	legoz = 0.0;
	angle = 0.0;
	scale = 1.0;

	olafx = 0.0;
	olafz = 0.0;
	olafangle = 0.0;

	snowballx = 0;
	snowbally = 0;
	snowballz = 0;
	snowballx0 = 0;
	snowbally0 = 0;
	snowballz0 = 0;

	firex = 0;
	firez = 0;

	firey1 = -30;
	firey2 = -30;
	firey3 = -30;
	firey4 = -30;
	firey5 = -30;

	pointx = 0;
	pointy = 0;
	pointz = 0;

	soldier1x = -13;
	soldier1y = 0;
	soldier1z = 13;

	soldier2x = 13;
	soldier2y = 0;
	soldier2z = -8;

	soldier3x = 0;
	soldier3y = 0;
	soldier3z = 20;

	soldierangle1 = 90; // 오른쪽 z --
	soldierangle2 = 270; // 왼쪽 z ++
	soldierangle3 = 180; // 위쪽 x --

	mouse_lstate = 1;
	mouse_rstate = 1;
	mouse_lx = 0;
	mouse_ly = 0;
	radius = 25.0f;
	theta = 42;
	phi = 39;

	centerx = 0;
	centery = 0;
	centerz = 0;

	saveObj.clear();
}
void calculate_eye() {
	eyex = radius * sin(theta * PI / 180) * cos(phi * PI / 180);
	eyey = radius * sin(phi * PI / 180);
	eyez = radius * cos(theta * PI / 180) * cos(phi * PI / 180);
	upVector = cos(phi * PI / 180);
	gluLookAt(eyex, eyey, eyez, centerx, centery, centerz, 0, upVector, 0);
}
void draw_axis(void) {
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glLineWidth(2);
	glBegin(GL_LINES);
	glColor3f(1, 0, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(4, 0, 0);

	glColor3f(0, 1, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 4, 0);

	glColor3f(0, 0, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 4);
	glEnd();
	glLineWidth(1);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
}
void draw_string(void* font, const char* str, float x_position, float y_position, float red, float green, float blue) {
	glPushAttrib(GL_LIGHTING_BIT);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(-10, 10, -10, 10);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glColor3f(red, green, blue);
	glRasterPos3f(x_position, y_position, 0);
	for (unsigned int i = 0; i < strlen(str); i++) {
		glutBitmapCharacter(font, str[i]);
	}
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopAttrib();
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
}
void draw_text() {
	glViewport(0, 0, 500, 500);
	glLoadIdentity();
	glColor3f(1, 1, 1);
	char buf[50] = "Computer Graphics Final Project";
	char buf2[50] = "12161781 ChangSuBeen";
	char buf3[50] = "Press the enter to start";
	draw_string(GLUT_BITMAP_HELVETICA_18, buf, 0.5, 5, 0, 0, 1);
	draw_string(GLUT_BITMAP_HELVETICA_18, buf2, 1.5, 3, 0, 0, 1);
	draw_string(GLUT_BITMAP_TIMES_ROMAN_24, buf3, 1, 0, 1, 0, 0);
}
void draw(void) {
	srand((unsigned int)time(NULL));
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); // 단위 벡터 초기화
	if (opening == 0) {
		gluLookAt(0, 10, 0, 0, 0, 0, 0, 0, 1);
		draw_text();
	}
	else if (lose == 1) {
		gluLookAt(0, 10, 0, 0, 0, 0, 0, 0, 1);
		glViewport(0, 0, 500, 500);
		glLoadIdentity();
		draw_string(GLUT_BITMAP_HELVETICA_18, "YOU LOSE", 4, 5, 1, 0, 0);
	}
	else if (win == 1) {
		gluLookAt(0, 10, 0, 0, 0, 0, 0, 0, 1);
		glViewport(0, 0, 500, 500);
		glLoadIdentity();
		draw_string(GLUT_BITMAP_HELVETICA_18, "YOU WIN", 4, 5, 0, 0, 1);
	}
	else {

		calculate_eye();
		glViewport(0, 0, 800, 600);

		glPushMatrix();
		glTranslatef(0, -10, 0);
		draw_skybox();
		glPopMatrix();
		glMatrixMode(GL_TEXTURE);
		glPushMatrix();

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		draw_square();
		glTranslatef(0, -70, 0);
		if(game_mode > 0)
			draw_textureCube();
		glPopMatrix();
		glMatrixMode(GL_TEXTURE);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);

		glPushMatrix();
		if (game_mode == 0) {
			light0();
			GLfloat light_pos0[] = { 10.0f, -10.0f, -10.0f, 1.0f };
			glLightfv(GL_LIGHT0, GL_POSITION, light_pos0);
			glPushMatrix();
			if (insert_mode == 1) {
				draw_axis();
				draw_rec();
				glViewport(200, 150, 800, 600);
				glMatrixMode(GL_MODELVIEW);
				legox = 0; legoy = 0; legoz = 0; angle = 0; scale = 1;
				draw_axis();
				draw_lego();
			}
			if (insert_mode2 == 1) {
				draw_axis();
				draw_rec();
				alpha = 0.5f;
				draw_lego();
				alpha = 1.0f;
			}
			glPopMatrix();
		}
		else {
			light1();
			GLfloat light_pos1[] = { 10.0f,10.0f,10.0f,1.0f };
			glLightfv(GL_LIGHT1, GL_POSITION, light_pos1);
			if (snow_mode) {
				draw_snow();
			}
			glPushMatrix();
			if (fire1)
				draw_particle1();
			glPopMatrix();
			glPushMatrix();
			if (fire2)
				draw_particle2();
			glPopMatrix();
			glPushMatrix();
			if (fire3)
				draw_particle3();
			glPopMatrix();
			glPushMatrix();
			if (fire4)
				draw_particle4();
			glPopMatrix();
			glPushMatrix();
			if (fire5)
				draw_particle5();
			glPopMatrix();
			glPushMatrix();
			draw_sphere();
			glPopMatrix();
			glPushMatrix();
			glTranslatef(0, -20, 0);
			glRotatef(-120, 0, 1, 0);
			glScalef(4.0f, 4.0f, 4.0f);
			draw_castle();
			glTranslatef(4, 0, 0);
			glPushMatrix();
			draw_olaf(); 
			glPushMatrix();
			if (game_mode == 2) {
				draw_soldier1();
				draw_soldier2();
				draw_soldier3();
				move_soldier();
				collision();
			}
			glPopMatrix();
			glPopMatrix();
		}
		glPopMatrix();
	}
	/*GLfloat tmp[16];
	glGetFloatv(GL_MODELVIEW, tmp);
	GLfloat tmp_inv[16];*/

	glutSwapBuffers();
}
void draw_obj(ObjParser* objParser) {
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_TRIANGLES);
	for (unsigned int n = 0; n < objParser->getFaceSize(); n += 3) {
		glNormal3f(objParser->normal[objParser->normalIdx[n] - 1].x,
			objParser->normal[objParser->normalIdx[n] - 1].y,
			objParser->normal[objParser->normalIdx[n] - 1].z);
		glVertex3f(objParser->vertices[objParser->vertexIdx[n] - 1].x,
			objParser->vertices[objParser->vertexIdx[n] - 1].y,
			objParser->vertices[objParser->vertexIdx[n] - 1].z);

		glNormal3f(objParser->normal[objParser->normalIdx[n + 1] - 1].x,
			objParser->normal[objParser->normalIdx[n + 1] - 1].y,
			objParser->normal[objParser->normalIdx[n + 1] - 1].z);
		glVertex3f(objParser->vertices[objParser->vertexIdx[n + 1] - 1].x,
			objParser->vertices[objParser->vertexIdx[n + 1] - 1].y,
			objParser->vertices[objParser->vertexIdx[n + 1] - 1].z);

		glNormal3f(objParser->normal[objParser->normalIdx[n + 2] - 1].x,
			objParser->normal[objParser->normalIdx[n + 2] - 1].y,
			objParser->normal[objParser->normalIdx[n + 2] - 1].z);
		glVertex3f(objParser->vertices[objParser->vertexIdx[n + 2] - 1].x,
			objParser->vertices[objParser->vertexIdx[n + 2] - 1].y,
			objParser->vertices[objParser->vertexIdx[n + 2] - 1].z);

	}
	glEnd();
	glEnable(GL_TEXTURE_2D);
}
void draw_obj_texture(ObjParser* objParser, GLuint texName) {
	glDisable(GL_BLEND);

	glBindTexture(GL_TEXTURE_2D, texName);
	glBegin(GL_TRIANGLES);
	for (unsigned int n = 0; n < objParser->getFaceSize(); n += 3) {
		glTexCoord2f(objParser->textures[objParser->textureIdx[n] - 1].x,
			objParser->textures[objParser->textureIdx[n] - 1].y);
		glNormal3f(objParser->normal[objParser->normalIdx[n] - 1].x,
			objParser->normal[objParser->normalIdx[n] - 1].y,
			objParser->normal[objParser->normalIdx[n] - 1].z);
		glVertex3f(objParser->vertices[objParser->vertexIdx[n] - 1].x,
			objParser->vertices[objParser->vertexIdx[n] - 1].y,
			objParser->vertices[objParser->vertexIdx[n] - 1].z);

		glTexCoord2f(objParser->textures[objParser->textureIdx[n + 1] - 1].x,
			objParser->textures[objParser->textureIdx[n + 1] - 1].y);
		glNormal3f(objParser->normal[objParser->normalIdx[n + 1] - 1].x,
			objParser->normal[objParser->normalIdx[n + 1] - 1].y,
			objParser->normal[objParser->normalIdx[n + 1] - 1].z);
		glVertex3f(objParser->vertices[objParser->vertexIdx[n + 1] - 1].x,
			objParser->vertices[objParser->vertexIdx[n + 1] - 1].y,
			objParser->vertices[objParser->vertexIdx[n + 1] - 1].z);

		glTexCoord2f(objParser->textures[objParser->textureIdx[n + 2] - 1].x,
			objParser->textures[objParser->textureIdx[n + 2] - 1].y);
		glNormal3f(objParser->normal[objParser->normalIdx[n + 2] - 1].x,
			objParser->normal[objParser->normalIdx[n + 2] - 1].y,
			objParser->normal[objParser->normalIdx[n + 2] - 1].z);
		glVertex3f(objParser->vertices[objParser->vertexIdx[n + 2] - 1].x,
			objParser->vertices[objParser->vertexIdx[n + 2] - 1].y,
			objParser->vertices[objParser->vertexIdx[n + 2] - 1].z);

	}
	glEnd();
	glEnable(GL_LIGHTING);
}

void draw_lego() {
	for (int i = 0; i < 9; i++) {
		if (color == i) {
			glColor4f(colorMatrix[i][0], colorMatrix[i][1], colorMatrix[i][2], alpha);
		}
	}
	for (int i = 0; i < OBJNUMBER; i++) {
		if (shape == i)
			lego = new ObjParser(objName[i]);
	}
	glTranslatef(legox, legoy, legoz);
	glRotatef(angle, 0, 1, 0);
	glScalef(scale, scale, scale);
	draw_obj(lego);
	glutPostRedisplay();
}
void draw_olaf(void) {
	glColor3f(1.0f, 1.0f, 1.0f);
	olaf = new ObjParser(objName3);
	glTranslatef(olafx, 0.0f, olafz);
	glRotatef(olafangle, 0, 1, 0);
	draw_obj_texture(olaf, texName2);
	glutPostRedisplay();
}

void draw_castle(void) {
	glColor3f(1.0f, 1.0f, 1.0f);
	for (int i = 0; i < 6; i++) {
		castle[i] = new ObjParser(objCastle[i]);
		draw_obj_texture(castle[i], texName3[i]);
	}
	glutPostRedisplay();
}
void draw_soldier1(void) {
	glColor3f(1.0f, 1.0f, 1.0f);
	soldier = new ObjParser(objName4);
	glTranslatef(soldier1x, soldier1y, soldier1z);
	draw_obj(soldier);
	glutPostRedisplay();
}
void draw_soldier2(void) {
	glColor3f(1.0f, 1.0f, 1.0f);
	soldier = new ObjParser(objName4);
	glTranslatef(soldier2x, soldier2y, soldier2z);
	draw_obj(soldier);
	glutPostRedisplay();
}
void draw_soldier3(void) {
	glColor3f(1.0f, 1.0f, 1.0f);
	soldier = new ObjParser(objName4);
	glTranslatef(soldier3x, soldier3y, soldier3z);
	draw_obj(soldier);
	glutPostRedisplay();
}
void draw_rec() {
	for (int i = 0; i < saveObj.size(); i++) {
		for (int j = 0; j < 9; j++) {
			if (saveObj[i].color == j) {
				glColor4f(colorMatrix[j][0], colorMatrix[j][1], colorMatrix[j][2], alpha);
			}
		}
		for (int j = 0; j < OBJNUMBER; j++) {
			if (saveObj[i].shape == j)
				lego = new ObjParser(objName[j]);
		}
		glPushMatrix();
		glTranslatef(saveObj[i].legox, saveObj[i].legoy, saveObj[i].legoz);
		glRotatef(saveObj[i].angle, 0, 1, 0);
		glScalef(saveObj[i].scale, saveObj[i].scale, saveObj[i].scale);
		draw_obj(lego);
		glPopMatrix();
	}
	glutPostRedisplay();
}
void draw_snow(void) {
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	for (int i = 0; i < snowNum; i++) {
		glPointSize(snow[i].size);
		glBegin(GL_POINTS);
		glColor4f(snow[i].color[0], snow[i].color[1], snow[i].color[2], snow[i].energy);
		glVertex3f(snow[i].position[0], snow[i].position[1], snow[i].position[2]);
		glEnd();
		snow[i].position[1] -= snow[i].velocity[1];
		if (snow[i].position[1] < -20.0f)
			snow[i].position[1] = 150.0f;

		snow[i].energy -= 0.01f;
	}
	glutPostRedisplay();

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
}
float br(float min, float max)
{
	return min + (rand() % 100) * (max - min) / 100;
}
void draw_snow_init(void) {
	float height = 150.0f;
	for (int i = 0; i < snowNum; i++) {
		snow[i].color[0] = 1;//br(0.0f,1.0f);
		snow[i].color[1] = 1;//br(0.0f,1.0f);
		snow[i].color[2] = 1;//br(0.0f,1.0f);

		snow[i].position[0] = br(2 * height, 0.1f) - height;
		snow[i].position[1] = br(2 * height, 0.1f) - height;
		snow[i].position[2] = br(2 * height, 0.1f) - height;

		snow[i].velocity[1] = br(1.0f, 2.0f);

		snow[i].size = br(5.0f, 10.0f);
		snow[i].energy = br(1.0f, 5.0f);
	}
}
void draw_particle1() {
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_LIGHTING);
	if (fire1) {
		glColor3f(1.0, 1.0, 1.0);
		glBindTexture(GL_TEXTURE_2D, texName4[2]);
		glTranslatef(-10.0f, firey1, 0.0f);
		gluSphere(qobj, 0.3f, 50, 50);
	}

	glDisable(GL_TEXTURE_2D);
	if (particle1) {
		for (int i = 0; i < particleNum; i++) {
			glPointSize(3.0f);
			glBegin(GL_POINTS);
			glColor4f(1, 0, 0, 0.6);
			pointx = br(-15, 15);
			pointy = br(-15, 15);
			pointz = br(-15, 15);
			if (pow(pointx, 2) + pow(pointy, 2) + pow(pointz, 2) < 225)
				glVertex3i(pointx, pointy, pointz);
			glEnd();
		}
	}
	glutPostRedisplay();

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
}

void draw_particle2() {
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_LIGHTING);

	if (fire2) {
		glColor3f(1.0, 1.0, 1.0);
		glBindTexture(GL_TEXTURE_2D, texName4[2]);
		glTranslatef(-25.0f, firey2, 10.0f);
		gluSphere(qobj, 0.3f, 50, 50);
	}

	glDisable(GL_TEXTURE_2D);
	if (particle2) {
		for (int i = 0; i < particleNum; i++) {
			glPointSize(3.0f);
			glBegin(GL_POINTS);
			glColor4f(0, 1, 0, 0.6f);
			pointx = br(-15, 15);
			pointy = br(-15, 15);
			pointz = br(-15, 15);
			if (pow(pointx, 2) + pow(pointy, 2) + pow(pointz, 2) < 225)
				glVertex3i(pointx, pointy, pointz);
			glEnd();
		}
	}
	glutPostRedisplay();

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
}
void draw_particle3() {
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_LIGHTING);


	if (fire3) {
		glColor3f(1.0, 1.0, 1.0);
		glBindTexture(GL_TEXTURE_2D, texName4[2]);
		glTranslatef(30.0f, firey3, -10.0f);
		gluSphere(qobj, 0.3f, 50, 50);
	}

	glDisable(GL_TEXTURE_2D);
	if (particle3) {
		for (int i = 0; i < particleNum; i++) {
			glPointSize(3.0f);
			glBegin(GL_POINTS);
			glColor4f(1, 1, 0, 0.6);
			pointx = br(-20, 20);
			pointy = br(-20, 20);
			pointz = br(-20, 20);
			if (pow(pointx, 2) + pow(pointy, 2) + pow(pointz, 2) < 400)
				glVertex3i(pointx, pointy, pointz);
			glEnd();
		}
	}
	glutPostRedisplay();

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
}
void draw_particle4() {
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_LIGHTING);

	if (fire4) {
		glColor3f(1.0, 1.0, 1.0);
		glBindTexture(GL_TEXTURE_2D, texName4[2]);
		glTranslatef(0.0f, firey4, -20.0f);
		gluSphere(qobj, 0.3f, 50, 50);
	}

	glDisable(GL_TEXTURE_2D);
	if (particle4) {
		for (int i = 0; i < particleNum; i++) {
			glPointSize(3.0f);
			glBegin(GL_POINTS);
			glColor4f(1, 0, 1, 0.6);
			pointx = br(-19, 19);
			pointy = br(-19, 19);
			pointz = br(-19, 19);
			if (pow(pointx, 2) + pow(pointy, 2) + pow(pointz, 2) < 361)
				glVertex3i(pointx, pointy, pointz);
			glEnd();
		}
	}
	glutPostRedisplay();

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
}
void draw_particle5() {
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_LIGHTING);

	if (fire5) {
		glColor3f(1.0, 1.0, 1.0);
		glBindTexture(GL_TEXTURE_2D, texName4[2]);
		glTranslatef(20.0f, firey5, 10.0f);
		gluSphere(qobj, 0.3f, 50, 50);
	}

	glDisable(GL_TEXTURE_2D);
	if (particle5) {
		for (int i = 0; i < particleNum; i++) {
			glPointSize(3.0f);
			glBegin(GL_POINTS);
			glColor4f(0, 1, 1, 0.6);
			pointx = br(-17, 17);
			pointy = br(-17, 17);
			pointz = br(-17, 17);
			if (pow(pointx, 2) + pow(pointy, 2) + pow(pointz, 2) < 289)
				glVertex3i(pointx, pointy, pointz);
			glEnd();
		}
	}
	glutPostRedisplay();

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
}

void draw_textureCube() {
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glDisable(GL_LIGHTING);

	glColor3f(0.5f, 0.5f, 0.5f);
	glScalef(100.0f, 50.0f, 100.0f);
	// left
	glBindTexture(GL_TEXTURE_2D, texName);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3f(-1.0f, 1.0f, -1.0f);
	glTexCoord2f(1, 0); glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(1, 1); glVertex3f(-1.0f, -1.0f, 1.0f);
	glTexCoord2f(0, 1); glVertex3f(-1.0f, 1.0f, 1.0f);
	glEnd();
	// right
	glBindTexture(GL_TEXTURE_2D, texName);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(1, 0); glVertex3f(1.0f, -1.0f, 1.0f);
	glTexCoord2f(1, 1); glVertex3f(1.0f, -1.0f, -1.0f);
	glTexCoord2f(0, 1); glVertex3f(1.0f, 1.0f, -1.0f);
	glEnd();
	// bottom
	glBindTexture(GL_TEXTURE_2D, texName);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(1, 0); glVertex3f(1.0f, -1.0f, -1.0f);
	glTexCoord2f(1, 1); glVertex3f(1.0f, -1.0f, 1.0f);
	glTexCoord2f(0, 1); glVertex3f(-1.0f, -1.0f, 1.0f);
	glEnd();
	// top
	glBindTexture(GL_TEXTURE_2D, texName);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3f(-1.0f, 1.0f, 1.0f);
	glTexCoord2f(1, 0); glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(1, 1); glVertex3f(1.0f, 1.0f, -1.0f);
	glTexCoord2f(0, 1); glVertex3f(-1.0f, 1.0f, -1.0f);
	glEnd();
	// near
	glBindTexture(GL_TEXTURE_2D, texName);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(1, 0); glVertex3f(-1.0f, 1.0f, 1.0f);
	glTexCoord2f(1, 1); glVertex3f(-1.0f, -1.0f, 1.0f);
	glTexCoord2f(0, 1); glVertex3f(1.0f, -1.0f, 1.0f);
	glEnd();
	// far
	glBindTexture(GL_TEXTURE_2D, texName);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3f(1.0f, 1.0f, -1.0f);
	glTexCoord2f(1, 0); glVertex3f(-1.0f, 1.0f, -1.0f);
	glTexCoord2f(1, 1); glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(0, 1); glVertex3f(1.0f, -1.0f, -1.0f);
	glEnd();
	glFlush();

	glEnable(GL_LIGHTING);
}
void draw_square() {
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_R);
	glEnable(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, g_nCubeTex);
	// Mesh DrawglEnable(GL_CLIP_PLANE1); 
	GLdouble eq1[4] = { 0.0,-1.0,0.0,1 };
	glClipPlane(GL_CLIP_PLANE1, eq1);
	glEnable(GL_CLIP_PLANE1);

	glDisable(GL_CLIP_PLANE1);
	//glutSolidSphere(10.0f, 30, 200);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_GEN_R);
	glDisable(GL_TEXTURE_CUBE_MAP);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
}
void draw_skybox() {
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glColor3f(1.0, 1.0, 1.0);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, g_nCubeTex);
	float g_nSkySize = 100.0f;
	glBegin(GL_QUADS);
	glTexCoord3d(1.0, -1.0, -1.0); glVertex3f(g_nSkySize, -g_nSkySize, -g_nSkySize);
	glTexCoord3d(1.0, -1.0, 1.0); glVertex3f(g_nSkySize, -g_nSkySize, g_nSkySize);
	glTexCoord3d(1.0, 1.0, 1.0); glVertex3f(g_nSkySize, g_nSkySize, g_nSkySize);
	glTexCoord3d(1.0, 1.0, -1.0); glVertex3f(g_nSkySize, g_nSkySize, -g_nSkySize);

	glTexCoord3d(-1.0, -1.0, -1.0); glVertex3f(-g_nSkySize, -g_nSkySize, -g_nSkySize);
	glTexCoord3d(-1.0, -1.0, 1.0); glVertex3f(-g_nSkySize, -g_nSkySize, g_nSkySize);
	glTexCoord3d(-1.0, 1.0, 1.0); glVertex3f(-g_nSkySize, g_nSkySize, g_nSkySize);
	glTexCoord3d(-1.0, 1.0, -1.0); glVertex3f(-g_nSkySize, g_nSkySize, -g_nSkySize);

	glTexCoord3d(1.0, 1.0, 1.0); glVertex3f(g_nSkySize, g_nSkySize, g_nSkySize);
	glTexCoord3d(-1.0, 1.0, 1.0); glVertex3f(-g_nSkySize, g_nSkySize, g_nSkySize);
	glTexCoord3d(-1.0, 1.0, -1.0); glVertex3f(-g_nSkySize, g_nSkySize, -g_nSkySize);
	glTexCoord3d(1.0, 1.0, -1.0); glVertex3f(g_nSkySize, g_nSkySize, -g_nSkySize);

	glTexCoord3d(1.0, -1.0, 1.0); glVertex3f(g_nSkySize, -g_nSkySize, g_nSkySize);
	glTexCoord3d(-1.0, -1.0, 1.0); glVertex3f(-g_nSkySize, -g_nSkySize, g_nSkySize);
	glTexCoord3d(-1.0, -1.0, -1.0); glVertex3f(-g_nSkySize, -g_nSkySize, -g_nSkySize);
	glTexCoord3d(1.0, -1.0, -1.0); glVertex3f(g_nSkySize, -g_nSkySize, -g_nSkySize);

	glTexCoord3d(1.0, -1.0, 1.0); glVertex3f(g_nSkySize, -g_nSkySize, g_nSkySize);
	glTexCoord3d(-1.0, -1.0, 1.0); glVertex3f(-g_nSkySize, -g_nSkySize, g_nSkySize);
	glTexCoord3d(-1.0, 1.0, 1.0); glVertex3f(-g_nSkySize, g_nSkySize, g_nSkySize);
	glTexCoord3d(1.0, 1.0, 1.0); glVertex3f(g_nSkySize, g_nSkySize, g_nSkySize);

	glTexCoord3d(1.0, -1.0, -1.0); glVertex3f(g_nSkySize, -g_nSkySize, -g_nSkySize);
	glTexCoord3d(-1.0, -1.0, -1.0); glVertex3f(-g_nSkySize, -g_nSkySize, -g_nSkySize);
	glTexCoord3d(-1.0, 1.0, -1.0); glVertex3f(-g_nSkySize, g_nSkySize, -g_nSkySize);
	glTexCoord3d(1.0, 1.0, -1.0); glVertex3f(g_nSkySize, g_nSkySize, -g_nSkySize);

	glEnd();

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
}
void draw_sphere() {
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glDisable(GL_LIGHTING);

	glRotatef(-60, 1, 1, 0);

	glColor3f(1.0, 1.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, texName4[0]);
	glTranslatef(20.0f, 30.0f, -10.0f);
	gluSphere(qobj, 0.5f, 50, 50);

	glBindTexture(GL_TEXTURE_2D, texName4[1]);
	glTranslatef(0.0f, 30.0f, 0.0f);
	gluSphere(qobj, 0.5f, 50, 50);

	glBindTexture(GL_TEXTURE_2D, texName4[2]);
	glTranslatef(5.0f, 0.0f, 10.0f);
	gluSphere(qobj, 0.5f, 50, 50);

	glBindTexture(GL_TEXTURE_2D, texName4[0]);
	glTranslatef(-40.0f, 0.0f, 5.0f);
	gluSphere(qobj, 0.5f, 50, 50);

	glBindTexture(GL_TEXTURE_2D, texName4[1]);
	glTranslatef(20.0f, 0.0f, 5.0f);
	gluSphere(qobj, 0.5f, 50, 50);

	glBindTexture(GL_TEXTURE_2D, texName4[2]);
	glTranslatef(-40.0f, 0.0f, 5.0f);
	gluSphere(qobj, 0.5f, 50, 50);

	glEnable(GL_LIGHTING);
}

void cubeTextureMapping() {
	glGenTextures(1, &texName);
	int width, height, channels;
	glBindTexture(GL_TEXTURE_2D, texName);
	uchar* img = readImageData("img/sea.bmp", &width, &height, &channels);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void myTextureMapping() {
	glGenTextures(1, &texName2);
	int width, height, channels;
	glBindTexture(GL_TEXTURE_2D, texName2);
	uchar* img = readImageData("img/olaf.bmp", &width, &height, &channels);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void castleTextureMapping() {
	glGenTextures(6, texName3);
	int width, height, channels;
	uchar* img0;
	if (roofTex == 1)
		img0 = readImageData("img/blue.bmp", &width, &height, &channels);
	else if (roofTex == 2)
		img0 = readImageData("img/blue2.bmp", &width, &height, &channels);
	else
		img0 = readImageData("img/blue3.bmp", &width, &height, &channels);
	uchar* img1 = readImageData("img/brown.bmp", &width, &height, &channels);
	uchar* img2 = readImageData("img/flags.bmp", &width, &height, &channels);
	uchar* img3;
	if (rampartTex == 1)
		img3 = readImageData("img/rampart.bmp", &width, &height, &channels);
	else if (rampartTex == 2)
		img3 = readImageData("img/rampart2.bmp", &width, &height, &channels);
	else
		img3 = readImageData("img/rampart3.bmp", &width, &height, &channels);
	uchar* img4 = readImageData("img/roof.bmp", &width, &height, &channels);
	uchar* img5 = readImageData("img/window.bmp", &width, &height, &channels);

	glBindTexture(GL_TEXTURE_2D, texName3[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, texName3[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, texName3[2]);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, texName3[3]);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img3);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, texName3[4]);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img4);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, texName3[5]);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img5);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}
void skyboxTextureMapping() {
	glGenTextures(1, &g_nCubeTex);
	int width, height, channels;
	/*uchar* img0 = readImageData("img/winterpx.bmp", &width, &height, &channels);
	uchar* img1 = readImageData("img/winternx.bmp", &width, &height, &channels);
	uchar* img2 = readImageData("img/winterpy.bmp", &width, &height, &channels);
	uchar* img3 = readImageData("img/winterny.bmp", &width, &height, &channels);
	uchar* img4 = readImageData("img/winterpz.bmp", &width, &height, &channels);
	uchar* img5 = readImageData("img/winternz.bmp", &width, &height, &channels);*/

	uchar* img0 = readImageData("img/nightpx.bmp", &width, &height, &channels);
	uchar* img1 = readImageData("img/nightnx.bmp", &width, &height, &channels);
	uchar* img2 = readImageData("img/nightpy.bmp", &width, &height, &channels);
	uchar* img3 = readImageData("img/nightny.bmp", &width, &height, &channels);
	uchar* img4 = readImageData("img/nightpz.bmp", &width, &height, &channels);
	uchar* img5 = readImageData("img/nightnz.bmp", &width, &height, &channels);


	glBindTexture(GL_TEXTURE_CUBE_MAP, g_nCubeTex);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img0);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img1);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img2);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img3);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img4);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img5);

	glBindTexture(GL_TEXTURE_CUBE_MAP, g_nCubeTex);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
	glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);

	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_R);
	glEnable(GL_TEXTURE_CUBE_MAP);
}

void sphereTextureMapping() {
	glGenTextures(3, texName4);
	int width, height, channels;
	uchar* img0 = readImageData("img/bright.bmp", &width, &height, &channels);
	uchar* img1 = readImageData("img/bright2.bmp", &width, &height, &channels);
	uchar* img2 = readImageData("img/fire.bmp", &width, &height, &channels);

	glBindTexture(GL_TEXTURE_2D, texName4[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, texName4[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, texName4[2]);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void save_legoblock() {
	saveObj.push_back({ shape, color, legox, legoy, legoz, angle, scale });
}

void save_lego() {
	if (saveObj.size() != 0) {
		ofstream out("data.txt", ios::trunc);
		if (out.is_open()) {
			for (int i = 0; i < saveObj.size(); i++) {
				out << saveObj[i].shape << endl;
				out << saveObj[i].color << endl;
				out << saveObj[i].legox << endl;
				out << saveObj[i].legoy << endl;
				out << saveObj[i].legoz << endl;
				out << saveObj[i].angle << endl;
				out << saveObj[i].scale << endl;
			}
			cout << "data save" << endl;
		}
		else cout << "파일이 없습니다." << endl;
		out.close();
	}
	else {
		cout << "저장할 lego 없음!" << endl;
	}
}
void open_lego() {
	ifstream in("data.txt");
	legoblock lb;
	if (in.is_open()) {
		for (int i = 0, j = 0;; j++) {
			if (in.eof()) break;
			in >> lb.shape;
			in >> lb.color;
			in >> lb.legox;
			in >> lb.legoy;
			in >> lb.legoz;
			in >> lb.angle;
			in >> lb.scale;
			saveObj.push_back(lb);
			if (j == 7) {
				i++;
				j = 0;
			}
		}
	}
	else cout << "파일이 없습니다." << endl;
	in.close();
}

void move_soldier() {
	int dir = rand() % 4 + 1; // 1 left 2 right 3 down 4 up
	if (dir == 1) {
		double temp = soldier1z;
		double angletemp = soldierangle1;
		soldier1z += 1.0f;
		soldierangle1 = 270;
		if (soldier1z < -10 || soldier1z > -4) {
			soldier1z = temp;
			soldierangle1 = angletemp;
			dir = rand() % 4 + 1;
		}
	}
	else if (dir == 2) {
		double temp = soldier1z;
		double angletemp = soldierangle1;
		soldier1z -= 1.0f;
		soldierangle1 = 90;
		if (soldier1z > 10 || soldier1z < 4) {
			soldier1z = temp;
			soldierangle1 = angletemp;
			dir = rand() % 4 + 1;
		}
	}
	else if (dir == 3) {
		double temp = soldier1x;
		double angletemp = soldierangle1;
		soldier1x += 1.0f;
		soldierangle1 = 360;
		if (soldier1x > 16 || soldier1x > 0) {
			soldier1x = temp;
			soldierangle1 = angletemp;
			dir = rand() % 4 + 1;
		}
	}
	else {
		double temp = soldier1x;
		double angletemp = soldierangle1;
		soldier1x -= 1.0f;
		soldierangle1 = 180;
		if (soldier1x < -5 || soldier1x < 8) {
			soldier1x = temp;
			soldierangle1 = angletemp;
			dir = rand() % 4 + 1;
		}
	}

	dir = rand() % 4 + 1; // 1 left 2 right 3 down 4 up
	if (dir == 1) {
		double temp = soldier2z;
		double angletemp = soldierangle2;
		soldier2z += 1.0f;
		soldierangle2 = 270;
		if (soldier2z < -13 || soldier2z > -4) {
			soldier2z = temp;
			soldierangle2 = angletemp;
			dir = rand() % 4 + 1;
		}
	}
	else if (dir == 2) {
		double temp = soldier2z;
		double angletemp = soldierangle2;
		soldier2z -= 1.0f;
		soldierangle2 = 90;
		if (soldier2z > 13 || soldier2z < 4) {
			soldier2z = temp;
			soldierangle2 = angletemp;
			dir = rand() % 4 + 1;
		}
	}
	else if (dir == 3) {
		double temp = soldier2x;
		double angletemp = soldierangle2;
		soldier2x += 1.0f;
		soldierangle2 = 360;
		if (soldier2z > 25 || soldier2z > 0) {
			soldier2z = temp;
			soldierangle2 = angletemp;
			dir = rand() % 4 + 1;
		}
	}
	else {
		double temp = soldier2x;
		double angletemp = soldierangle2;
		soldier2x -= 1.0f;
		soldierangle2 = 180;
		if (soldier2z < -8 || soldier2z < 8) {
			soldier2z = temp;
			soldierangle2 = angletemp;
			dir = rand() % 4 + 1;
		}
	}
	dir = rand() % 4 + 1; // 1 left 2 right 3 down 4 up
	if (dir == 1) {
		double temp = soldier3z;
		double angletemp = soldierangle3;
		soldier3z += 1.0f;
		soldierangle3 = 270;
		if (soldier3z < -13 || soldier3z > -4) {
			soldier3z = temp;
			soldierangle3 = angletemp;
			dir = rand() % 4 + 1;
		}
	}
	else if (dir == 2) {
		double temp = soldier3z;
		double angletemp = soldierangle3;
		soldier3z -= 1.0f;
		soldierangle3 = 90;
		if (soldier3z > 13 || soldier3z < 4) {
			soldier3z = temp;
			soldierangle3 = angletemp;
			dir = rand() % 4 + 1;
		}
	}
	else if (dir == 3) {
		double temp = soldier3x;
		double angletemp = soldierangle3;
		soldier3x += 1.0f;
		soldierangle3 = 360;
		if (soldier3z > 25 || soldier3z > 0) {
			soldier3z = temp;
			soldierangle3 = angletemp;
			dir = rand() % 4 + 1;
		}
	}
	else {
		double temp = soldier3x;
		double angletemp = soldierangle3;
		soldier3x -= 1.0f;
		soldierangle3 = 180;
		if (soldier3z < -8 || soldier3z < 8) {
			soldier3z = temp;
			soldierangle3 = angletemp;
			dir = rand() % 4 + 1;
		}
	}
}
void collision() {
	if (lose == 0 && win == 0 && (abs(soldier1x - olafx) < 2 && abs(soldier1z - olafz) < 2) || (abs(soldier2x - olafx) < 2 && abs(soldier2z - olafz) < 1) || (abs(soldier3x - olafx) < 1 && abs(soldier3z - olafz) < 1)) {
		lose = 1;
		sndPlaySoundA(TEXT("BOOM.WAV"), SND_ASYNC | SND_NODEFAULT);
	}
	else if (lose == 0 && win == 0 && olafx >= -6 && olafx <= -2 && olafz >= -2 && olafz <= 2) {
		win = 1;
		sndPlaySoundA(TEXT("YEAH.wav"), SND_ASYNC | SND_NODEFAULT);
	}
}