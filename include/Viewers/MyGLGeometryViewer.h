#ifndef MYGLGEOMETRYVIEWER_H
#define MYGLGEOMETRYVIEWER_H

#include <GL/glew.h>
#include <GL/glut.h>

enum
{
	CUBE_FRONT = 0,
	CUBE_RIGHT = 1,
	CUBE_LEFT = 2,
	CUBE_BACK = 3, 
	CUBE_UP = 4,
	CUBE_DOWN = 5
};

class MyGLGeometryViewer
{

public:
	MyGLGeometryViewer();
	void configureSkyBoxAmbient(GLfloat *eye, GLfloat *at, GLfloat *up);
	void configureVolumeAmbient(GLfloat *eye, GLfloat *at, GLfloat *up);
	void configureLight();
	void loadBufQuad(GLuint *VBO, float x, float y, float z);
	void drawQuad(GLuint *VBO);
	void drawSkyBox(GLuint *skybox, float x, float y, float z);
	
};

#endif