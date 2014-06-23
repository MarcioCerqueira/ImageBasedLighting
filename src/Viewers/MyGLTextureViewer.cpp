#include "Viewers\MyGLTextureViewer.h"

MyGLTextureViewer::MyGLTextureViewer() 
{
	frameBuffer = (unsigned char*) malloc(640 * 480 * 3 * sizeof(unsigned char));
}

MyGLTextureViewer::~MyGLTextureViewer()
{
	delete [] frameBuffer;
}

void MyGLTextureViewer::loadRGBTexture(float *data, GLuint *texVBO, int index, int width, int height)
{
	glBindTexture(GL_TEXTURE_2D, texVBO[index]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, data);
}

void MyGLTextureViewer::loadRGBTexture(const unsigned char *data, GLuint *texVBO, int index, int imageWidth, int imageHeight)
{

	glBindTexture(GL_TEXTURE_2D, texVBO[index]);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

}

void MyGLTextureViewer::loadRGBATexture(unsigned char *data, GLuint *texVBO, int index, int imageWidth, int imageHeight)
{

	glBindTexture(GL_TEXTURE_2D, texVBO[index]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

}
void MyGLTextureViewer::load3DTextureFromTIFFile(unsigned char *data, GLuint *texVBO, int index, int imageWidth, int imageHeight, int numberOfSlices, GLint filter)
{
	
	glBindTexture(GL_TEXTURE_3D, texVBO[index]);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, filter);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, filter);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, imageWidth, imageHeight, numberOfSlices, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	
}

void MyGLTextureViewer::loadDepthComponentTexture(float *data, GLuint *texVBO, int index, int imageWidth, int imageHeight)
{

	glBindTexture(GL_TEXTURE_2D, texVBO[index]);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, imageWidth, imageHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, data);

}

void MyGLTextureViewer::loadFrameBufferTexture(GLuint *texVBO, int index, int x, int y, int width, int height)
{

	glReadPixels(x, y, width, height, GL_RGB, GL_UNSIGNED_BYTE, frameBuffer);
	
	glBindTexture(GL_TEXTURE_2D, texVBO[index]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, frameBuffer);

}

void MyGLTextureViewer::drawRGBTextureOnShader(GLuint *texVBO, int index, GLuint shaderProg, int imageWidth, int imageHeight)
{

	glUseProgram(shaderProg);
	
	gluOrtho2D( 0, imageWidth, imageHeight, 0 ); 
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	GLuint texLoc = glGetUniformLocation(shaderProg, "image");
	glUniform1i(texLoc, 0);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texVBO[index]);

	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); 
		glVertex2f(0.0f, 0.0f);
		glTexCoord2f(1.0f, 0.0f); 
		glVertex2f(imageWidth, 0.0f);
		glTexCoord2f(1.0f, 1.0f); 
		glVertex2f(imageWidth, imageHeight);
		glTexCoord2f(0.0f, 1.0f); 
		glVertex2f(0.0f, imageHeight);
	glEnd();

	glActiveTexture(GL_TEXTURE0);
	glDisable(GL_TEXTURE_2D);
	
	glUseProgram(0);

}
void MyGLTextureViewer::draw2DTexture(GLuint *texVBO, int index, GLuint shaderProg, int windowWidth, int windowHeight) {

	glUseProgram(shaderProg);

	GLuint texLoc = glGetUniformLocation(shaderProg, "image");
	glUniform1i(texLoc, 0);

	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texVBO[index]);
	
}

void MyGLTextureViewer::draw3DTexture(GLuint *texVBO, GLuint *VBO, GLuint shaderProg, int windowWidth, int windowHeight, 
	VRParams vrparams, HDRParams hdrparams) {

	glUseProgram(shaderProg);

	GLuint texLoc = glGetUniformLocation(shaderProg, "volume");
	glUniform1i(texLoc, 7);

	glActiveTexture(GL_TEXTURE7);
	glEnable(GL_TEXTURE_3D);
	glBindTexture(GL_TEXTURE_3D, texVBO[0]);

	texLoc = glGetUniformLocation(shaderProg, "minMaxOctree");
	glUniform1i(texLoc, 2);

	glActiveTexture(GL_TEXTURE2);
	glEnable(GL_TEXTURE_3D);
	glBindTexture(GL_TEXTURE_3D, texVBO[2]);

	texLoc = glGetUniformLocation(shaderProg, "stepSize");
	glUniform1f(texLoc, vrparams.stepSize);

	texLoc = glGetUniformLocation(shaderProg, "windowWidth");
	glUniform1i(texLoc, windowWidth);

	texLoc = glGetUniformLocation(shaderProg, "windowHeight");
	glUniform1i(texLoc, windowHeight);

	if(vrparams.useTransferFunction) {
		texLoc = glGetUniformLocation(shaderProg, "useTransferFunction");
		glUniform1i(texLoc, 1);
	} else {
		texLoc = glGetUniformLocation(shaderProg, "useTransferFunction");
		glUniform1i(texLoc, 0);
	}
	
	drawSHCoeffs(shaderProg, hdrparams);

	if(vrparams.useIBL) {
		texLoc = glGetUniformLocation(shaderProg, "useIBL");
		glUniform1i(texLoc, 1);
	} else {
		texLoc = glGetUniformLocation(shaderProg, "useIBL");
		glUniform1i(texLoc, 0);
	}

	drawQuads(5.0, 5.0, 5.0);

	texLoc = glGetUniformLocation(shaderProg, "transferFunction");
	glUniform1i(texLoc, 3);

	glActiveTexture(GL_TEXTURE3);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texVBO[1]);
		
	texLoc = glGetUniformLocation(shaderProg, "backFrameBuffer");
	glUniform1i(texLoc, 5);

	glActiveTexture(GL_TEXTURE5);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texVBO[3]);
	
	texLoc = glGetUniformLocation(shaderProg, "frontFrameBuffer");
	glUniform1i(texLoc, 6);

	glActiveTexture(GL_TEXTURE6);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texVBO[4]);

	glUseProgram(0);
	
	glActiveTexture(GL_TEXTURE2);
	glDisable(GL_TEXTURE_3D);
	glActiveTexture(GL_TEXTURE3);
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE5);
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE6);
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE7);
	glDisable(GL_TEXTURE_3D);

}

void MyGLTextureViewer::drawFinalRendering(GLuint texVolume, GLuint texScene, GLuint shaderProg, int windowWidth, int windowHeight) {

	glUseProgram(shaderProg);
	
	gluOrtho2D( 0, windowWidth, windowHeight, 0 ); 
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	
	GLuint texLoc = glGetUniformLocation(shaderProg, "volume");
	glUniform1i(texLoc, 0);
	texLoc = glGetUniformLocation(shaderProg, "scene");
	glUniform1i(texLoc, 1);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texVolume);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texScene);
	
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); 
		glVertex2f(0.0f, 0.0f);
		glTexCoord2f(1.0f, 0.0f); 
		glVertex2f(windowWidth, 0.0f);
		glTexCoord2f(1.0f, 1.0f); 
		glVertex2f(windowWidth, windowHeight);
		glTexCoord2f(0.0f, 1.0f); 
		glVertex2f(0.0f, windowHeight);
	glEnd();

	glUseProgram(0);

	glActiveTexture(GL_TEXTURE0);
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE1);
	glDisable(GL_TEXTURE_2D);
	
}
	
void MyGLTextureViewer::drawSHCoeffs(GLuint shaderProg, HDRParams params) {

	GLuint texLoc = glGetUniformLocation(shaderProg, "L00");
	glUniform3f(texLoc, params.SHCoeffs[0 * 3 + 0], params.SHCoeffs[0 * 3 + 1], params.SHCoeffs[0 * 3 + 2]);
	texLoc = glGetUniformLocation(shaderProg, "L1m1");
	glUniform3f(texLoc, params.SHCoeffs[1 * 3 + 0], params.SHCoeffs[1 * 3 + 1], params.SHCoeffs[1 * 3 + 2]);
	texLoc = glGetUniformLocation(shaderProg, "L10");
	glUniform3f(texLoc, params.SHCoeffs[2 * 3 + 0], params.SHCoeffs[2 * 3 + 1], params.SHCoeffs[2 * 3 + 2]);
	texLoc = glGetUniformLocation(shaderProg, "L11");
	glUniform3f(texLoc, params.SHCoeffs[3 * 3 + 0], params.SHCoeffs[3 * 3 + 1], params.SHCoeffs[3 * 3 + 2]);
	texLoc = glGetUniformLocation(shaderProg, "L2m2");
	glUniform3f(texLoc, params.SHCoeffs[4 * 3 + 0], params.SHCoeffs[4 * 3 + 1], params.SHCoeffs[4 * 3 + 2]);
	texLoc = glGetUniformLocation(shaderProg, "L2m1");
	glUniform3f(texLoc, params.SHCoeffs[5 * 3 + 0], params.SHCoeffs[5 * 3 + 1], params.SHCoeffs[5 * 3 + 2]);
	texLoc = glGetUniformLocation(shaderProg, "L20");
	glUniform3f(texLoc, params.SHCoeffs[6 * 3 + 0], params.SHCoeffs[6 * 3 + 1], params.SHCoeffs[6 * 3 + 2]);
	texLoc = glGetUniformLocation(shaderProg, "L21");
	glUniform3f(texLoc, params.SHCoeffs[7 * 3 + 0], params.SHCoeffs[7 * 3 + 1], params.SHCoeffs[7 * 3 + 2]);
	texLoc = glGetUniformLocation(shaderProg, "L22");
	glUniform3f(texLoc, params.SHCoeffs[8 * 3 + 0], params.SHCoeffs[8 * 3 + 1], params.SHCoeffs[8 * 3 + 2]);
	texLoc = glGetUniformLocation(shaderProg, "lightDir");
	glUniform3f(texLoc, params.dominantLightDirection[0], params.dominantLightDirection[1], params.dominantLightDirection[2]);
	texLoc = glGetUniformLocation(shaderProg, "lightColor");
	glUniform3f(texLoc, params.dominantLightColor[0], params.dominantLightColor[1], params.dominantLightColor[2]);
	texLoc = glGetUniformLocation(shaderProg, "diffuseScaleFactor");
	glUniform1f(texLoc, params.diffuseScaleFactor);
	texLoc = glGetUniformLocation(shaderProg, "specularScaleFactor");
	glUniform1f(texLoc, params.specularScaleFactor);
	texLoc = glGetUniformLocation(shaderProg, "shininess");
	glUniform1f(texLoc, params.shininess);
	
}

void MyGLTextureViewer::drawQuads(float x, float y, float z, GLenum target) {
	
	bool color = true;
	glBegin(GL_QUADS);
	//front
	if(color) glColor3f(0, 1, 1);
	glMultiTexCoord3f(target, 0.0f, 1.0f, 1.0f);
	glVertex3f(-x, y, z);	//0
	if(color) glColor3f(0, 0, 1);
	glMultiTexCoord3f(target, 0.0f, 0.0f, 1.0f);
	glVertex3f(-x, -y, z);	//1
	if(color) glColor3f(1, 0, 1);
	glMultiTexCoord3f(target, 1.0f, 0.0f, 1.0f);
	glVertex3f(x, -y, z);	//2
	if(color) glColor3f(1, 1, 1);
	glMultiTexCoord3f(target, 1.0f, 1.0f, 1.0f);
	glVertex3f(x, y, z);	//3
	
	//left
	if(color) glColor3f(0, 1, 0);
	glMultiTexCoord3f(target, 0.0f, 1.0f, 0.0f);
	glVertex3f(-x, y, -z);	//4
	if(color) glColor3f(0, 0, 0);
	glMultiTexCoord3f(target, 0.0f, 0.0f, 0.0f);
	glVertex3f(-x, -y, -z);	//5
	if(color) glColor3f(0, 0, 1);
	glMultiTexCoord3f(target, 0.0f, 0.0f, 1.0f);
	glVertex3f(-x, -y, z);	//1
	if(color) glColor3f(0, 1, 1);
	glMultiTexCoord3f(target, 0.0f, 1.0f, 1.0f);
	glVertex3f(-x, y, z);	//0

	//back
	if(color) glColor3f(1, 1, 0);
	glMultiTexCoord3f(target, 1.0f, 1.0f, 0.0f);
	glVertex3f(x, y, -z);	//7
	if(color) glColor3f(1, 0, 0);
	glMultiTexCoord3f(target, 1.0f, 0.0f, 0.0f);
	glVertex3f(x, -y, -z);	//6
	if(color) glColor3f(0, 0, 0);
	glMultiTexCoord3f(target, 0.0f, 0.0f, 0.0f);
	glVertex3f(-x, -y, -z);	//5
	if(color) glColor3f(0, 1, 0);
	glMultiTexCoord3f(target, 0.0f, 1.0f, 0.0f);
	glVertex3f(-x, y, -z);	//4

	//right
	if(color) glColor3f(1, 1, 1);
	glMultiTexCoord3f(target, 1.0f, 1.0f, 1.0f);
	glVertex3f(x, y, z);	//3
	if(color) glColor3f(1, 0, 1);
	glMultiTexCoord3f(target, 1.0f, 0.0f, 1.0f);
	glVertex3f(x, -y, z);	//2
	if(color) glColor3f(1, 0, 0);
	glMultiTexCoord3f(target, 1.0f, 0.0f, 0.0f);
	glVertex3f(x, -y, -z);	//6
	if(color) glColor3f(1, 1, 0);
	glMultiTexCoord3f(target, 1.0f, 1.0f, 0.0f);
	glVertex3f(x, y, -z);	//7

	//top
	if(color) glColor3f(0, 1, 0);
	glMultiTexCoord3f(target, 0.0f, 1.0f, 0.0f);
	glVertex3f(-x, y, -z);	//4
	if(color) glColor3f(0, 1, 1);
	glMultiTexCoord3f(target, 0.0f, 1.0f, 1.0f);
	glVertex3f(-x, y, z);	//0
	if(color) glColor3f(1, 1, 1);
	glMultiTexCoord3f(target, 1.0f, 1.0f, 1.0f);
	glVertex3f(x, y, z);	//3
	if(color) glColor3f(1, 1, 0);
	glMultiTexCoord3f(target, 1.0f, 1.0f, 0.0f);
	glVertex3f(x, y, -z);	//7

	//bottom
	if(color) glColor3f(1, 0, 0);
	glMultiTexCoord3f(target, 1.0f, 0.0f, 0.0f);
	glVertex3f(x, -y, -z);	//6
	if(color) glColor3f(1, 0, 1);
	glMultiTexCoord3f(target, 1.0f, 0.0f, 1.0f);
	glVertex3f(x, -y, z);	//2
	if(color) glColor3f(0, 0, 1);
	glMultiTexCoord3f(target, 0.0f, 0.0f, 1.0f);
	glVertex3f(-x, -y, z);	//1
	if(color) glColor3f(0, 0, 0);
	glMultiTexCoord3f(target, 0.0f, 0.0f, 0.0f);
	glVertex3f(-x, -y, -z);	//5
	
	glEnd();

}