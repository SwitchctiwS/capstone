#include "../inc/main.hpp"

// Namespaces
using namespace cv;

// Global variables
bool fromCenter = false;
bool showCrosshair = true;
int frameWidth = 480;
int frameHeight = 360;

int main(int argc, char** argv) {
	// Help text
	if (argc > 1 ) {
		helpText();
		return -1;
	}

	// Declarations
	Rect2d roi;
	Mat frame;
	Ptr<Tracker> tracker = Tracker::create("KCF");
	::Point centerOfRoi;
	
	// Setup
	// Open webcam at resolution
	VideoCapture cap(0);
	if (!cap.isOpened()) {
		return -1;
	}
	cap.set(CV_CAP_PROP_FRAME_WIDTH, frameWidth);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, frameHeight);

	// Select ROI from first image
	cap >> frame;
	roi = selectROI("CompBot View", frame, showCrosshair, fromCenter);

	// Return if no image selected
	if (roi.width == 0 || roi.height == 0) {
		return -1;
	}

	// Initialize tracker using ROI from picture
	tracker->init(frame, roi);

	std::cout << "Press ESC to quit." << std::endl;

	// Loop
	while (true) {
		if (waitKey(1) == 27) {
			break;
		}

		// Get frame from webcam
		cap >> frame;

		// Update tracking result
		tracker->update(frame, roi);

		// Draw green box around tracked object
		rectangle(frame, roi, Scalar(0, 255, 0), 2, 1);
		imshow("CompBot View", frame);

		// Find center of ROI on Cartesian plane
		centerOfRoi.x = (roi.x + roi.width / 2) - frameWidth / 2;
		centerOfRoi.y = -((roi.y + roi.height / 2) - frameHeight / 2);
		std::cout << "X: " << centerOfRoi.x << " Y: " << centerOfRoi.y << std::endl;
	}

	// Free webcam
	cap.release();

	return 0;
}

void helpText(void) {
	std::cout << 
		"Usage: CompBot\n"
		"\n"
			"\te.g.: CompBot\n"
		"\n"
		"That's really it.\n" 
		<< std::endl;
}

