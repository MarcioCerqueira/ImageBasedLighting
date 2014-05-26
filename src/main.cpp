#include <opencv2/opencv.hpp>
#include <GL/glew.h>
#include <GL/glut.h>
#include <fstream>
#include <string>
#include "Viewers\MyGLTextureViewer.h"
#include "Viewers\MyGLGeometryViewer.h"
#include "Viewers\shader.h"
#include "Viewers\glm.h"
#include "IO\loadsave.h"
#include "VolumeRendering\VRParams.h"
#include "VolumeRendering\Volume.h"
#include "VolumeRendering\MinMaxOctree.h"
#include "VolumeRendering\TransferFunction.h"

//Window size
int windowWidth = 640;
int windowHeight = 480;

//  The number of frames
int frameCount = 0;
float fps = 0;
int currentTime = 0, previousTime = 0;

cv::Mat cubeMap;
cv::Mat cubeFaces[6];
cv::Mat hdrMap;

GLuint texCube[6];
GLuint texVolume[10];
GLuint bufQuad[4];
GLuint texHDR[1];
GLuint texScene[5];
GLuint frontQuadFrameBuffer;
GLuint backQuadFrameBuffer;
GLuint volumeFrameBuffer;
GLuint sceneFrameBuffer;

GLMmodel* model;
MyGLTextureViewer *myGLTextureViewer;
MyGLGeometryViewer myGLGeometryViewer;
VRParams vrparams;
Volume *volume;
MinMaxOctree *minMaxOctree;
TransferFunction *transferFunction;

GLfloat eye[3] = { 0.0, 0.0, 4.0 };
GLfloat at[3]  = { 0.0, 0.0, 0.0 };
GLfloat up[3]  = { 0.0, 1.0, -1.0 };

float translationVector[3] = {0.0, 0.0, 0.0};
float rotationAngles[3] = {0.0, 0.0, 180.0};
int scale = 5;
int vel = 1;

bool translation = false;
bool rotation = false;

GLuint ProgramObject = 0;
GLuint VertexShaderObject = 0;
GLuint FragmentShaderObject = 0;
GLuint shaderVS, shaderFS, shaderProg[5];   // handles to objects
GLint  linked;

int mode = 0;

void calculateFPS()
{

	frameCount++;
	currentTime = glutGet(GLUT_ELAPSED_TIME);

    int timeInterval = currentTime - previousTime;

    if(timeInterval > 1000)
    {
        fps = frameCount / (timeInterval / 1000.0f);
        previousTime = currentTime;
        frameCount = 0;
		std::cout << fps << std::endl;
    }

}

void reshape(int w, int h)
{
	
	windowWidth = w;
	windowHeight = h;

	glViewport( 0, 0, windowWidth, windowHeight );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluOrtho2D( 0, windowWidth, 0, windowHeight );
	glMatrixMode( GL_MODELVIEW );
   
}

void loadArguments(int argc, char **argv) {

	std::fstream file(argv[1]);
	std::string line;
	
	if(file.is_open()) {

		std::getline(file, line);
		std::cout << line << std::endl;
		std::getline(file, line);
		std::cout << line << std::endl;
		cubeMap = cv::imread(line.c_str());	
		std::getline(file, line);
		std::cout << line << std::endl;

		std::getline(file, line);
		std::cout << line << std::endl;
		std::getline(file, line);
		std::cout << line << std::endl;
		hdrMap = cv::hdrImread(line.c_str(), CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_COLOR);
		std::getline(file, line);
		std::cout << line << std::endl;
		
		std::getline(file, line);
		std::cout << line << std::endl;
		std::getline(file, line);
		std::cout << line << std::endl;
		model = glmReadOBJ((char*)line.c_str());
		std::getline(file, line);
		std::cout << line << std::endl;

		std::getline(file, line);
		std::cout << line << std::endl;
		std::getline(file, line);
		std::cout << line << std::endl;
		std::fstream volumeConfigFile(line.c_str());
		if(volumeConfigFile.is_open()) {
			
			std::getline(volumeConfigFile, line);
			strcpy(vrparams.volumePath, line.c_str());
			std::getline(volumeConfigFile, line);
			strcpy(vrparams.transferFunctionPath, line.c_str());
			std::getline(volumeConfigFile, line);
			vrparams.firstSlice = atoi(line.c_str());
			std::getline(volumeConfigFile, line);
			vrparams.lastSlice = atoi(line.c_str());
			std::getline(volumeConfigFile, line);
			strcpy(vrparams.volumeExtension, line.c_str());
			std::getline(volumeConfigFile, line);
			vrparams.scaleWidth = atof(line.c_str());
			std::getline(volumeConfigFile, line);
			vrparams.scaleHeight = atof(line.c_str());
			std::getline(volumeConfigFile, line);
			vrparams.scaleDepth = atof(line.c_str());
			std::getline(volumeConfigFile, line);
			vrparams.rotationX = atoi(line.c_str());
			std::getline(volumeConfigFile, line);
			vrparams.rotationY = atoi(line.c_str());
			std::getline(volumeConfigFile, line);
			vrparams.rotationZ = atoi(line.c_str());
			
		}

		cv::cvtColor(cubeMap, cubeMap, CV_BGR2RGB);
		cv::cvtColor(hdrMap, hdrMap, CV_BGR2RGB);
	}

}

void loadVolumeData() {

	volume = new Volume();

	if(!strcmp(vrparams.volumeExtension, "tif"))
		volume->loadTIFData(vrparams.volumePath, vrparams.firstSlice, vrparams.lastSlice);
	else if(!strcmp(vrparams.volumeExtension, "pgm"))
		volume->loadPGMData(vrparams.volumePath, vrparams.firstSlice, vrparams.lastSlice);
	else
		volume->loadRAWData(vrparams.volumePath, vrparams.firstSlice, vrparams.firstSlice, vrparams.lastSlice);

	minMaxOctree = new MinMaxOctree(volume->getWidth(), volume->getHeight(), volume->getDepth());
	minMaxOctree->build(volume->getData(), volume->getWidth(), volume->getHeight(), volume->getDepth());
	
	transferFunction = new TransferFunction();
	transferFunction->load(vrparams.transferFunctionPath);
	transferFunction->computePreIntegrationTable();
	
}

void displayQuadForVolumeRendering(bool front) 
{

	glViewport(0, 0, windowWidth, windowHeight);
	
    myGLGeometryViewer.configureSkyBoxAmbient(eye, at, up);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glPushMatrix();
	
	glTranslatef(translationVector[0] - 2.0, translationVector[1], translationVector[2] - 4.0);
	glRotatef(rotationAngles[vrparams.rotationX] - 85.0, 1, 0, 0);
	glRotatef(rotationAngles[vrparams.rotationY] + 10.0, 0, 1, 0);
	glRotatef(rotationAngles[vrparams.rotationZ] - 15, 0, 0, 1);
	
	//draw volume - 1st part
	
	if(!front) {
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
	} else {
		glDisable(GL_CULL_FACE);
	}
	
	myGLGeometryViewer.drawQuad(bufQuad);
	glPopMatrix();

}

void displayMedicalVolume()
{

	glViewport( 0, 0, windowWidth, windowHeight );
	
	myGLGeometryViewer.configureVolumeAmbient(eye, at, up);

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();

	myGLTextureViewer->draw3DTexture(texVolume, bufQuad, texHDR[0], shaderProg[1], windowWidth, windowHeight, vrparams);
	
	glActiveTexture(GL_TEXTURE0);
	glDisable(GL_TEXTURE_2D);
	
}


void displayPolygonalScene()
{

	glViewport( 0, 0, windowWidth, windowHeight );

	myGLGeometryViewer.configureSkyBoxAmbient(eye, at, up);

	glMatrixMode(GL_MODELVIEW);
	glTranslatef(translationVector[0], translationVector[1], translationVector[2]);
	glRotatef(0, 1, 0, 0);
	glRotatef(0, 0, 1, 0);
	glRotatef(180.0, 0, 0, 1);
	
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glScalef(scale, scale, scale);
	myGLGeometryViewer.drawSkyBox(texCube, 1.0, 1.0, 1.0);
	glPopMatrix();

	glPushMatrix();
	glRotatef(-180, 0, 0, 1);
	glEnable(GL_LIGHTING);

	myGLTextureViewer->draw2DTexture(texHDR, 0, shaderProg[0], windowWidth, windowHeight);
	GLuint texLoc = glGetUniformLocation(shaderProg[0], "mode");
	glUniform1i(texLoc, mode);
	
	//draw sphere
	glutSolidSphere(1.0, 50, 40);
	
	//draw obj
	glTranslatef(2.0, 0.0, 0.0);
	glmDraw(model, GLM_SMOOTH | GLM_MATERIAL);
	
	glUseProgram(0);

}

void display()
{

	glBindFramebuffer(GL_FRAMEBUFFER, backQuadFrameBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	displayQuadForVolumeRendering(false);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, frontQuadFrameBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	displayQuadForVolumeRendering(true);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, volumeFrameBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	displayMedicalVolume();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, sceneFrameBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	displayPolygonalScene();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	glViewport( 0, 0, windowWidth, windowHeight );
	glClear( GL_COLOR_BUFFER_BIT  | GL_DEPTH_BUFFER_BIT );
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	myGLTextureViewer->drawFinalRendering(texVolume[7], texScene[0], shaderProg[3], windowWidth, windowHeight);

	glutSwapBuffers();
	glutPostRedisplay();

}

void idle()
{
	calculateFPS();
}

void keyboard(unsigned char key, int x, int y) 
{

	translation = false;
	rotation = false;
	switch(key) {
	case 27:
		exit(0);
		break;
	case 't':
		translation = true;
		break;
	case 'r':
		rotation = true;
		break;
	case 's':
		scale++;
		break;
	case 'm':
		if(mode == 0) mode = 1;
		else mode = 0;
		vrparams.useIBL = mode;
		break;
	case 'f':
		vrparams.useTransferFunction = !vrparams.useTransferFunction;
		break;
	}
}

void specialKeyboard(int key, int x, int y)
{
	switch(key) {
	case GLUT_KEY_UP:
		if(translation)
			translationVector[1] += vel;
		if(rotation)
			rotationAngles[1] += 5 * vel;
		break;
	case GLUT_KEY_DOWN:
		if(translation)
			translationVector[1] -= vel;
		if(rotation)
			rotationAngles[1] -= 5 * vel;
		break;
	case GLUT_KEY_LEFT:
		if(translation)
			translationVector[0] -= vel;
		if(rotation)
			rotationAngles[0] -= 5 * vel;
		break;
	case GLUT_KEY_RIGHT:
		if(translation)
			translationVector[0] += vel;
		if(rotation)
			rotationAngles[0] += 5 * vel;
		break;
	case GLUT_KEY_PAGE_UP:
		if(translation)
			translationVector[2] += vel;
		if(rotation)
			rotationAngles[2] += 5 * vel;
		break;
	case GLUT_KEY_PAGE_DOWN:
		if(translation)
			translationVector[2] -= vel;
		if(rotation)
			rotationAngles[2] -= 5 * vel;
		std::cout << rotationAngles[0] << " " << rotationAngles[1] << " " << rotationAngles[2] << std::endl;
		break;
	default:
		break;
	}
}

void initGL() {
	
	glClearColor(0.0f, 0.0f, 0.0f, 0.0);
	glShadeModel(GL_SMOOTH);
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1);  

	if(texCube[0] == 0)
		glGenTextures(6, texCube);
	if(texHDR[0] == 0)
		glGenTextures(1, texHDR);
	if(texVolume[0] == 0)
		glGenTextures(10, texVolume);
	if(texScene[0] == 0)
		glGenTextures(5, texScene);
	if(bufQuad[0] == 0)
		glGenBuffers(4, bufQuad);
	if(frontQuadFrameBuffer == 0)
		glGenFramebuffers(1, &frontQuadFrameBuffer);
	if(backQuadFrameBuffer == 0)
		glGenFramebuffers(1, &backQuadFrameBuffer);
	if(volumeFrameBuffer == 0)
		glGenFramebuffers(1, &volumeFrameBuffer);
	if(sceneFrameBuffer == 0)
		glGenFramebuffers(1, &sceneFrameBuffer);
	
	myGLTextureViewer = new MyGLTextureViewer();

	//Load cube faces
	int faceWidth = cubeMap.cols/3;
	int faceHeight = cubeMap.rows/4;

	cubeFaces[CUBE_UP] = cv::Mat(faceHeight, faceWidth, cubeMap.type());
	
	int oldPixel = 0;
	int newPixel = 0;

	for(int y = 0; y < faceHeight; y++) {
		for(int x = faceWidth; x < 2 * faceWidth; x++) {
			oldPixel = y * cubeMap.cols + x;
			newPixel = y * faceWidth + (x - faceWidth);
			for(int ch = 0; ch < 3; ch++)
				cubeFaces[CUBE_UP].ptr<unsigned char>()[newPixel * 3 + ch] = cubeMap.ptr<unsigned char>()[oldPixel * 3 + ch];
		}
	}

	cubeFaces[CUBE_LEFT] = cv::Mat(faceHeight, faceWidth, cubeMap.type());

	for(int y = faceHeight; y < 2 * faceHeight; y++) {
		for(int x = 0; x < faceWidth; x++) {
			oldPixel = y * cubeMap.cols + x;
			newPixel = (y - faceHeight) * faceWidth + x;
			for(int ch = 0; ch < 3; ch++)
				cubeFaces[CUBE_LEFT].ptr<unsigned char>()[newPixel * 3 + ch] = cubeMap.ptr<unsigned char>()[oldPixel * 3 + ch];
		}
	}

	cubeFaces[CUBE_FRONT] = cv::Mat(faceHeight, faceWidth, cubeMap.type());

	for(int y = faceHeight; y < 2 * faceHeight; y++) {
		for(int x = faceWidth; x < 2 * faceWidth; x++) {
			oldPixel = y * cubeMap.cols + x;
			newPixel = (y - faceHeight) * faceWidth + (x - faceWidth);
			for(int ch = 0; ch < 3; ch++)
				cubeFaces[CUBE_FRONT].ptr<unsigned char>()[newPixel * 3 + ch] = cubeMap.ptr<unsigned char>()[oldPixel * 3 + ch];
		}
	}

	cubeFaces[CUBE_RIGHT] = cv::Mat(faceHeight, faceWidth, cubeMap.type());

	for(int y = faceHeight; y < 2 * faceHeight; y++) {
		for(int x = 2 * faceWidth; x < 3 * faceWidth; x++) {
			oldPixel = y * cubeMap.cols + x;
			newPixel = (y - faceHeight) * faceWidth + (x - 2 * faceWidth);
			for(int ch = 0; ch < 3; ch++)
				cubeFaces[CUBE_RIGHT].ptr<unsigned char>()[newPixel * 3 + ch] = cubeMap.ptr<unsigned char>()[oldPixel * 3 + ch];
		}
	}

	cubeFaces[CUBE_DOWN] = cv::Mat(faceHeight, faceWidth, cubeMap.type());

	for(int y = 2 * faceHeight; y < 3 * faceHeight; y++) {
		for(int x = faceWidth; x < 2 * faceWidth; x++) {
			oldPixel = y * cubeMap.cols + x;
			newPixel = (y - 2 * faceHeight) * faceWidth + (x - faceWidth);
			for(int ch = 0; ch < 3; ch++)
				cubeFaces[CUBE_DOWN].ptr<unsigned char>()[newPixel * 3 + ch] = cubeMap.ptr<unsigned char>()[oldPixel * 3 + ch];
		}
	}

	cubeFaces[CUBE_BACK] = cv::Mat(faceHeight, faceWidth, cubeMap.type());

	for(int y = 3 * faceHeight; y < 4 * faceHeight; y++) {
		for(int x = faceWidth; x < 2 * faceWidth; x++) {
			oldPixel = y * cubeMap.cols + x;
			newPixel = (y - 3 * faceHeight) * faceWidth + (x - faceWidth);
			for(int ch = 0; ch < 3; ch++)
				cubeFaces[CUBE_BACK].ptr<unsigned char>()[newPixel * 3 + ch] = cubeMap.ptr<unsigned char>()[oldPixel * 3 + ch];
		}
	}

	myGLTextureViewer->loadRGBTexture(cubeFaces[CUBE_FRONT].ptr<unsigned char>(), texCube, CUBE_FRONT, faceWidth, faceHeight);
	myGLTextureViewer->loadRGBTexture(cubeFaces[CUBE_BACK].ptr<unsigned char>(), texCube, CUBE_BACK, faceWidth, faceHeight);
	myGLTextureViewer->loadRGBTexture(cubeFaces[CUBE_UP].ptr<unsigned char>(), texCube, CUBE_UP, faceWidth, faceHeight);
	myGLTextureViewer->loadRGBTexture(cubeFaces[CUBE_DOWN].ptr<unsigned char>(), texCube, CUBE_DOWN, faceWidth, faceHeight);
	myGLTextureViewer->loadRGBTexture(cubeFaces[CUBE_LEFT].ptr<unsigned char>(), texCube, CUBE_LEFT, faceWidth, faceHeight);
	myGLTextureViewer->loadRGBTexture(cubeFaces[CUBE_RIGHT].ptr<unsigned char>(), texCube, CUBE_RIGHT, faceWidth, faceHeight);
	myGLTextureViewer->loadRGBTexture(hdrMap.ptr<float>(), texHDR, 0, hdrMap.cols, hdrMap.rows);

	myGLTextureViewer->load3DTextureFromTIFFile(volume->getData(), texVolume, 0, volume->getWidth(), volume->getHeight(), volume->getDepth(), GL_LINEAR);
	myGLTextureViewer->load3DTextureFromTIFFile(minMaxOctree->getData(), texVolume, 2, minMaxOctree->getWidth(), minMaxOctree->getHeight(), minMaxOctree->getDepth(), GL_LINEAR);
	myGLTextureViewer->loadRGBATexture(transferFunction->getPreIntegrationTable(), texVolume, 1, 256, 256);
	
	myGLTextureViewer->loadRGBTexture((unsigned char*)NULL, texVolume, 3, windowWidth, windowHeight);
	myGLTextureViewer->loadRGBTexture((unsigned char*)NULL, texVolume, 4, windowWidth, windowHeight);
	myGLTextureViewer->loadRGBTexture((unsigned char*)NULL, texVolume, 7, windowWidth, windowHeight);
	myGLTextureViewer->loadRGBTexture((unsigned char*)NULL, texScene, 0, windowWidth, windowHeight);

	myGLTextureViewer->loadDepthComponentTexture(NULL, texVolume, 5, windowWidth, windowHeight);
	myGLTextureViewer->loadDepthComponentTexture(NULL, texVolume, 6, windowWidth, windowHeight);
	myGLTextureViewer->loadDepthComponentTexture(NULL, texVolume, 8, windowWidth, windowHeight);
	myGLTextureViewer->loadDepthComponentTexture(NULL, texScene, 1, windowWidth, windowHeight);

	myGLGeometryViewer.loadBufQuad(bufQuad, 1.0f/vrparams.scaleWidth, 1.0f/vrparams.scaleHeight, 1.0f/vrparams.scaleDepth);

	glBindFramebuffer(GL_FRAMEBUFFER, frontQuadFrameBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texVolume[6], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texVolume[4], 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
		std::cout << "FBO OK" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, backQuadFrameBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texVolume[5], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texVolume[3], 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
		std::cout << "FBO OK" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, volumeFrameBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texVolume[8], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texVolume[7], 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
		std::cout << "FBO OK" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, sceneFrameBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texScene[1], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texScene[0], 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
		std::cout << "FBO OK" << std::endl;

	vrparams.stepSize = 0.004;//1.0/50.0;
	vrparams.useTransferFunction = false;
	vrparams.useIBL = false;
	vrparams.IBLScaleFactor = 0.075;

	glmUnitize(model);
    glmFacetNormals(model);
    glmVertexNormals(model, 90.0);

	myGLGeometryViewer.configureLight();
	
}

int main(int argc, char **argv) {

	if(argc < 2) {
		std::cout << "IBL.exe configFile.txt" << std::endl;
		return 0;
	}

	loadArguments(argc, argv);
	loadVolumeData();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_ALPHA);
	glutInitWindowSize(windowWidth, windowHeight);
	glutCreateWindow("Image-Based Lighting");

	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialKeyboard);
	
	glewInit();
	initGL();

	initShader("Shaders/IBL", 0);
	initShader("Shaders/VRRaycasting", 1);
	initShader("Shaders/VRImage", 2);
	initShader("Shaders/FinalRendering", 3);
	glUseProgram(0);

	glutMainLoop();

	delete volume;
	delete minMaxOctree;
	delete transferFunction;
	delete myGLTextureViewer;
	return 0;

}