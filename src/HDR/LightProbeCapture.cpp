#include <HDR\LightProbeCapture.h>


void mouseCallbackFunc(int event, int x, int y, int flags, void* userData)
{

	if(event == cv::EVENT_RBUTTONDOWN) {
		lightProbeCenter[0] = x;
		lightProbeCenter[1] = y;
	}

}

LightProbeCapture::LightProbeCapture() {

	capture = new cv::VideoCapture(0);
	if(!capture->isOpened()) std::cout << "Capture error" << std::endl;
	cv::namedWindow("Light Probe Capture");
	cv::setMouseCallback("Light Probe Capture", mouseCallbackFunc, NULL);
	lightProbeCenter[0] = 320;
	lightProbeCenter[1] = 240;
	lightProbeSize = 100;

}

LightProbeCapture::~LightProbeCapture() {
	
	delete capture;

}

void LightProbeCapture::captureSphericalMap() {

	*capture >> image;
	cv::resize(image, image, cv::Size(640, 480));
	
	//Segment light probe
	unsigned char *pointer = image.ptr<unsigned char>();
	for(int y = 0; y < 480; y++) {
		for(int x = 0; x < 640; x++) {
			int pixel = y * 640 + x;
			if(y < lightProbeCenter[1] - lightProbeSize || y > lightProbeCenter[1] + lightProbeSize || 
			   x < lightProbeCenter[0] - lightProbeSize || x > lightProbeCenter[0] + lightProbeSize) {
				for(int ch = 0; ch < 3; ch++)
					pointer[pixel * 3 + ch] = 0;
			}
		}
	}
	
	cv::imshow("Light Probe Capture", image);

}