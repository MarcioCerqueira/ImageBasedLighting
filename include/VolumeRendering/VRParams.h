#ifndef VRPARAMS_H
#define VRPARAMS_H

typedef struct VRParams
{
	//step size for raycasting
	float stepSize;
	//scale factors
	float scaleWidth;
	float scaleHeight;
	float scaleDepth;
	//rotation axis
	int rotationX;
	int rotationY;
	int rotationZ;
	char transferFunctionPath[100];	
	char volumePath[1000];
	int firstSlice;
	int lastSlice;
	char volumeExtension[4];
	bool useTransferFunction;
	bool useIBL;
	float IBLScaleFactor;

} VRParams;

#endif