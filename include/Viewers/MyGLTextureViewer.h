#ifndef MYGLTEXTUREVIEWER_H
#define MYGLTEXTUREVIEWER_H

#include <GL/glew.h>
#include <GL/glut.h>
#include <malloc.h>
#include "VolumeRendering\VRParams.h"

class MyGLTextureViewer
{

public:
	MyGLTextureViewer();
	~MyGLTextureViewer();
	void loadRGBTexture(float *data, GLuint *texVBO, int index, int width, int height);
	void loadRGBTexture(const unsigned char *data, GLuint *texVBO, int index, int imageWidth, int imageHeight);
	void loadRGBATexture(unsigned char *data, GLuint *texVBO, int index, int imageWidth, int imageHeight);
	void load3DTextureFromTIFFile(unsigned char *data, GLuint *texVBO, int index, int imageWidth, int imageHeight, int numberOfSlices, GLint filter);
	void loadDepthComponentTexture(float *data, GLuint *texVBO, int index, int windowWidth, int windowHeight);
	void loadFrameBufferTexture(GLuint *texVBO, int index, int x, int y, int width, int height);
	void drawRGBTextureOnShader(GLuint *texVBO, int index, GLuint shaderProg, int imageWidth, int imageHeight);
	void draw2DTexture(GLuint *texVBO, int index, GLuint shaderProg, int windowWidth, int windowHeight);
	void draw3DTexture(GLuint *texVBO, GLuint *VBO, float *SHCoeffs, GLuint shaderProg, int windowWidth, int windowHeight, VRParams params);
	void drawFinalRendering(GLuint texVolume, GLuint texScene, GLuint shaderProg, int windowWidth, int windowHeight);
	void drawSHCoeffs(GLuint shaderProg, float *SHCoeffs);
	void drawQuads(float x, float y, float z, GLenum target = GL_TEXTURE0);
private:
	unsigned char *frameBuffer;
};

#endif