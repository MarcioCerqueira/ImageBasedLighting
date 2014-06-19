#ifndef LIGHTPROBECAPTURE_H
#define LIGHTPROBECAPTURE_H

#include <opencv2\opencv.hpp>

static int lightProbeCenter[2];

class LightProbeCapture
{
public:
	LightProbeCapture();
	~LightProbeCapture();
	void captureSphericalMap();
	void incrementLightProbeSize() { lightProbeSize += 10; }
	cv::Mat getImage() { return image; }
private:
	cv::VideoCapture *capture;
	int lightProbeSize;
	cv::Mat image;
};

#endif