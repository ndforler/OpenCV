#define _CRT_SECURE_NO_WARNINGS
#include <opencv2/opencv.hpp>
#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui/highgui_c.h>

using namespace std;
using namespace cv;

//thses will be changed using trackbars
int MinH = 0;
int MaxH = 0;

int MinS = 0;
int MaxS = 255;

int MinV = 0;
int MaxV = 255;

int MinC = 120;
int MaxC = 150;

//Window Names
const string windowName = "Original";
const string windowName1 = "HSV Image";
const string windowName2 = "Threashold Image";
const string AdjustmentWindow = "Fine Tuning";

bool Road = true;

void on_trackbar(int, void*) {}; //Fimction called when track bar position gets moved

void setRoad(int MinHue = 70, int MaxHue = 179, int MinSat = 0, int MaxSat = 87, int MinValue = 0, int MaxValue = 77, int MinCanny = 50, int MaxCanny = 150)
{
	MinH = MinHue;
	MaxH = MaxHue;
	MinS = MinSat;
	MaxS = MaxSat;
	MinV = MinValue;
	MaxV = MaxValue;
	MinC = MinCanny;
	MaxC = MaxCanny;
}

void createTrackBars() 
{
	if (Road == true){ setRoad(); }
	namedWindow(AdjustmentWindow, 0);
	//create memory to store track bar names on the window
	char TrackBarName[50];
	//Create trackbars to insert into a window to fine tune values
	sprintf(TrackBarName, "MinH", MinH);
	sprintf(TrackBarName, "MaxH", MaxH);
	sprintf(TrackBarName, "MinS", MinS);
	sprintf(TrackBarName, "MaxS", MaxS);
	sprintf(TrackBarName, "MinV", MinV);
	sprintf(TrackBarName, "MaxV", MaxV);
	//Canny Threasholds
	sprintf(TrackBarName, "MinCanny", MinC);
	sprintf(TrackBarName, "MaxCanny", MaxC);

	//3 parameters are address variables that changes when the tracking is changed
	//Max and Min values tracking
	//The function that is called when vars are moved
	createTrackbar("MinH", AdjustmentWindow, &MinH, MaxH, on_trackbar);
	createTrackbar("MaxH", AdjustmentWindow, &MaxH, MaxH, on_trackbar);
	createTrackbar("MinS", AdjustmentWindow, &MinS, MaxS, on_trackbar);
	createTrackbar("MaxS", AdjustmentWindow, &MaxS, MaxS, on_trackbar);
	createTrackbar("MinV", AdjustmentWindow, &MinV, MaxV, on_trackbar);
	createTrackbar("MinV", AdjustmentWindow, &MaxV, MaxV, on_trackbar);


	// Canny
	createTrackbar("MinC", AdjustmentWindow, &MinC, MaxC, on_trackbar);
	createTrackbar("MaxC", AdjustmentWindow, &MaxC, MaxC, on_trackbar);
}

int main()
{
	VideoCapture capture("roadLane.mp4");

	if (!capture.isOpened())
	{
		cout << "error";
		return -1;
	}

	createTrackBars(); // Calls and creates the trackbars slider box

	Mat threshold, capt_Image, HSV_Img, dst;

	while (1)
	{
		// Step 1: Get the image/video/camera feed
		capture >> capt_Image;

		resize(capt_Image, capt_Image, Size(500, 640));

		// Here we define our region of interest box (x, y, b, c);
		//(x,y) =  (100, 295)
		// and the second corner is (x + b, y + c) = (100 + 220, 295 + 185)
		Rect const box(100, 295, 320, 185);

		// Step 2: Define a box
		Mat ROI = capt_Image(box); // Region of interest

		// Step 3: Canny
		// Edge detection using canny detector
		GaussianBlur(ROI, dst, Size(3, 3), 1.5, 1.5);

		// Morphological Operations
		Mat k = getStructuringElement(MORPH_RECT, Size(3, 3));
		morphologyEx(ROI, ROI, MORPH_CLOSE, k);

		Canny(ROI, dst, MinC, MaxC, 3);

		// Step 4: Convert image to HSV
		cvtColor(ROI, HSV_Img, COLOR_RGB2HSV);
		inRange(ROI, Scalar(70, 179, 0), Scalar(87, 0, 77), threshold);

		// Step 5: Use a vector to get HoughLinesP
		// Now applying hough transform to detect lines in our image
		vector<Vec4i> lines;
		double rho = 1;
		double theta = CV_PI / 180;
		int threshold = 50;
		double minLineLength = 10;
		double maxLineGap = 300;
		HoughLinesP(dst, lines, rho, theta, threshold, minLineLength, maxLineGap);
		// Draw the actual lines on the screen over the road
		int linethickness = 10;
		int lineType = LINE_4;

		for (size_t i = 0; i < lines.size(); i++)
		{
			Vec4i l = lines[i];
			Scalar greenColor = Scalar(0, 250, 0); /// B=0 G=250 R=0
			line(ROI, Point(l[0], l[1]), Point(l[2], l[3]), greenColor, linethickness, lineType);
		}

		imshow(windowName, capt_Image);
		imshow(windowName1, HSV_Img);
		imshow(windowName2, ROI);

		if (waitKey(20) == 27)
		{
			cout << "esc";
			break;
		}
	}
}

