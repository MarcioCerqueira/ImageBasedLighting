#ifndef HDRIMAGE_H
#define HDRIMAGE_H

#include <malloc.h>
#include <math.h>
#include <stdio.h>
#include <memory.h>
#include <cuda_runtime.h>

#define PI 3.1415

class HDRImage
{
public:
	HDRImage(int width, int height);
	~HDRImage();
	void computeCoordinates();
	void computeDomegaProduct();
	void computeSHCoeffs();
	void computeSphericalMap();
	void load(float *image);
	void load(unsigned char *image);
	float* getImage() { return image; }
	float* getSHCoeffs() { return SHCoeffs; }
	void setScale(float scale) { this->scale = scale; }

private:
	float *image;
	float *cartesianCoord;
	float *sphericalCoord;
	float *domegaProduct;
	float SHCoeffs[27];
	int width;
	int height;
	float scale;

};

#endif