#pragma once

#include "stdafx.h"
#include "jsonlib.h"
#include "Recognition2.h"
#include "Recognition3.h"
//#include "Recognition4.h"


// Функция детектирования знаков 
// подбор по размеру и наименьшему расстоянию до точек прямоугольника
// деление на - круглый и прямоугольный знак

//void Bound1(int numCadr, std::vector<std::vector<cv::Point> > squares, cv::Mat& src_mat)


int Bound_circle(int numCadr, cv::Mat& bgr_image, std::vector<std::vector<cv::Point> > squares); //!!!!!!!!!!!!!!!!! circle
void Bound_rect(int numcadr, cv::Mat& img); //!!!!!!!!!!!!!!!!!! rectangle


void setLabel(cv::Mat& im, const std::string label, std::vector<cv::Point>& contour);

// Функция детектирования знаков 
// подбор по размеру и наименьшему расстоянию до точек прямоугольника
// деление на - круглый и прямоугольный знак

void Bound1(int numCadr, std::vector<std::vector<cv::Point> > squares, cv::Mat& src_mat)
{
	Bound_circle(numCadr,src_mat,squares);
	Bound_rect(numCadr, src_mat);

	return;

}

// ***************************************************

double angle(cv::Point pt1, cv::Point pt2, cv::Point pt0)
{
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1 * dx2 + dy1 * dy2) / sqrt((dx1 * dx1 + dy1 * dy1) * (dx2 * dx2 + dy2 * dy2) + 1e-10);
}

//void (std::vector<std::vector<cv::Point> >)findSquaresInImage:(cv::Mat)_image
//{
//	std::vector<std::vector<cv::Point> > squares;
//	cv::Mat pyr, timg, gray0(_image.size(), CV_8U), gray;
//	int thresh = 50, N = 11;
//	cv::pyrDown(_image, pyr, cv::Size(_image.cols / 2, _image.rows / 2));
//	cv::pyrUp(pyr, timg, _image.size());
//	std::vector<std::vector<cv::Point> > contours;
//	for (int c = 0; c < 3; c++) {
//		int ch[] = { c, 0 };
//		mixChannels(&timg, 1, &gray0, 1, ch, 1);
//		for (int l = 0; l < N; l++) {
//			if (l == 0) {
//				cv::Canny(gray0, gray, 0, thresh, 5);
//				cv::dilate(gray, gray, cv::Mat(), cv::Point(-1, -1));
//			}
//			else {
//				gray = gray0 >= (l + 1) * 255 / N;
//			}
//			cv::findContours(gray, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
//			std::vector<cv::Point> approx;
//			for (size_t i = 0; i < contours.size(); i++)
//			{
//				cv::approxPolyDP(cv::Mat(contours[i]), approx, arcLength(cv::Mat(contours[i]), true) * 0.02, true);
//				if (approx.size() == 4 && fabs(contourArea(cv::Mat(approx))) > 1000 && cv::isContourConvex(cv::Mat(approx))) {
//					double maxCosine = 0;
//
//					for (int j = 2; j < 5; j++)
//					{
//						double cosine = fabs(angle(approx[j % 4], approx[j - 2], approx[j - 1]));
//						maxCosine = MAX(maxCosine, cosine);
//					}
//
//					if (maxCosine < 0.3) {
//						squares.push_back(approx);
//					}
//				}
//			}
//		}
//	}
//	return squares;
//}

void find_squares(cv::Mat& image, std::vector<std::vector<cv::Point> >& squares)
{
	// blur will enhance edge detection
	cv::Mat blurred(image);
	medianBlur(image, blurred, 9);

	cv::Mat gray0(blurred.size(), CV_8U), gray;
	std::vector<std::vector<cv::Point> > contours;

	// find squares in every color plane of the image
	for (int c = 0; c < 3; c++)
	{
		int ch[] = { c, 0 };
		mixChannels(&blurred, 1, &gray0, 1, ch, 1);

		// try several threshold levels
		const int threshold_level = 2;
		for (int l = 0; l < threshold_level; l++)
		{
			// Use Canny instead of zero threshold level!
			// Canny helps to catch squares with gradient shading
			if (l == 0)
			{
				Canny(gray0, gray, 10, 20, 3); // 

				// Dilate helps to remove potential holes between edge segments
				dilate(gray, gray, cv::Mat(), cv::Point(-1, -1));
			}
			else
			{
				gray = gray0 >= (l + 1) * 255 / threshold_level;
			}

			// Find contours and store them in a list

			int CV_RETR_LIST = 2;
			int CV_CHAIN_APPROX_SIMPLE = 2;
			findContours(gray, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

			// Test contours
			std::vector<cv::Point> approx;
			for (size_t i = 0; i < contours.size(); i++)
			{
				// approximate contour with accuracy proportional
				// to the contour perimeter
				cv::approxPolyDP(cv::Mat(contours[i]), approx, cv::arcLength(cv::Mat(contours[i]), true) * 0.02, true);

				// Note: absolute value of an area is used because
				// area may be positive or negative - in accordance with the
				// contour orientation
				if (approx.size() == 4 &&
					fabs(cv::contourArea(cv::Mat(approx))) > 1000 &&
					cv::isContourConvex(cv::Mat(approx)))
				{
					double maxCosine = 0;

					for (int j = 2; j < 5; j++)
					{
						double cosine = fabs(angle(approx[j % 4], approx[j - 2], approx[j - 1]));
						maxCosine = MAX(maxCosine, cosine);
					}

					if (maxCosine < 0.3)
						squares.push_back(approx);
				}
			}
		}
	}
}

void CTempoDoc::debugSquares(std::vector<std::vector<cv::Point> > squares, cv::Mat image)
{
	//return;
	

	for (int i = 0; i < squares.size(); i++) {
		// draw contour
		//cv::drawContours(image, squares, i, cv::Scalar(255, 0, 0), 1, 8, std::vector<cv::Vec4i>(), 0, cv::Point());

		// draw bounding rect
		cv::Rect rect = boundingRect(cv::Mat(squares[i]));
		cv::rectangle(image, rect.tl(), rect.br(), cv::Scalar(0, 255, 0), 2, 8, 0);

		// draw rotated rect
		cv::RotatedRect minRect = minAreaRect(cv::Mat(squares[i]));
		cv::Point2f rect_points[4];
		minRect.points(rect_points);
		for (int j = 0; j < 4; j++) {
			//cv::line(image, rect_points[j], rect_points[(j + 1) % 4], cv::Scalar(0, 0, 255), 1, 8); // blue
		}
	}

	//vector<vector<cv::Point> > contours;

	//findContours(image, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
	//cv::drawContours(image, contours, 0, cv::Scalar(255, 0, 0), 1, 8, std::vector<cv::Vec4i>(), 0, cv::Point());
	////cv::RotatedRect rotRecs[contours.size()];
	//for (int i = 0; i < contours.size(); i++) {
	//	//rotRecs[i] = fitEllipse(contours[i]);
	//}

	//return image;
}


static double angle2(cv::Point pt1, cv::Point pt2, cv::Point pt0)
{
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1 * dx2 + dy1 * dy2) / sqrt((dx1 * dx1 + dy1 * dy1) * (dx2 * dx2 + dy2 * dy2) + 1e-10);
}




// ***************************************************

void setLabel(cv::Mat& im, const std::string label, std::vector<cv::Point>& contour)
{
	return;
	//if (label != "CIR")return;

	int fontface = cv::FONT_HERSHEY_SIMPLEX;
	double scale = 0.4;
	int thickness = 1;
	int baseline = 0;

	cv::Size text = cv::getTextSize(label, fontface, scale, thickness, &baseline);
	cv::Rect r = cv::boundingRect(contour);

	cv::Point pt(r.x + ((r.width - text.width) / 2), r.y + ((r.height + text.height) / 2));
	//cv::rectangle(im, pt + cv::Point(0, baseline), pt + cv::Point(text.width, -text.height), CV_RGB(255, 255, 255), cv::FILLED);
	cv::putText(im, label, pt, fontface, scale, CV_RGB(255, 0, 0), thickness, 8);
}

// **********************************************

bool r_center(float radius, int cx, int cy, std::vector<std::vector<cv::Point> > squares)
{

	double r = 0;
	double S = 0;
	// считаем среднее расстояние от центра окружности до оболочки
	for (auto& m : squares)
	{
		for (int i = 0; i < 2; i++)
			r += (cx - m[i].x) * (cx - m[i].x) + (cy - m[i].y) * (cy - m[i].y);
	}
	//if (radius > 30 && radius < 40 && 40 < sqrt(r) && sqrt(r) < 100) return true;
	if ((50 < sqrt(r) && sqrt(r) < 140)) return true;

	return false;
}



// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
int Bound_circle(int numCadr, cv::Mat& bgr_image,  std::vector<std::vector<cv::Point> > squares) {

	cv::Mat orig_image = bgr_image.clone();

	cv::medianBlur(bgr_image, bgr_image, 3);

	// Convert input image to HSV
	cv::Mat hsv_image;
	cv::cvtColor(bgr_image, hsv_image, cv::COLOR_BGR2HSV);

	// Threshold the HSV image, keep only the red pixels
	cv::Mat lower_red_hue_range;
	cv::Mat upper_red_hue_range;
	cv::inRange(hsv_image, cv::Scalar(0, 100, 100), cv::Scalar(10, 255, 255), lower_red_hue_range);
	cv::inRange(hsv_image, cv::Scalar(160, 100, 100), cv::Scalar(179, 255, 255), upper_red_hue_range);

	// Combine the above two images
	cv::Mat red_hue_image;
	cv::addWeighted(lower_red_hue_range, 1.0, upper_red_hue_range, 1.0, 0.0, red_hue_image);

	cv::GaussianBlur(red_hue_image, red_hue_image, cv::Size(9, 9), 2, 2);

	// Use the Hough transform to detect circles in the combined threshold image
	std::vector<cv::Vec3f> circles;
	cv::HoughCircles(red_hue_image, circles, cv::HOUGH_GRADIENT, 1, red_hue_image.rows / 8, 100, 20, 0, 0);

	// Loop over all detected circles and outline them on the original image
	//if (circles.size() == 0) std::exit(-1);
	for (size_t current_circle = 0; current_circle < circles.size(); ++current_circle) {
		cv::Point center(std::round(circles[current_circle][0]), std::round(circles[current_circle][1]));
		int radius = std::round(circles[current_circle][2]);

		//cv::circle(orig_image, center, radius, cv::Scalar(0, 255, 0), 5);

		if (!r_center(radius, center.x, center.y, squares))
		{
			continue;
		}


		cv::Scalar red(0, 0, 255);
		cv::circle(bgr_image, center, radius, red, 3);
		std::vector<cv::Point> coordTectCapture = 
		{ cv::Point(center.x, center.y), 
			cv::Point(center.x - 70, center.y-170) };
		setLabel(bgr_image, "Circle Road sign", coordTectCapture);
	}

	// Show images
	/*cv::namedWindow("Threshold lower image", cv::WINDOW_AUTOSIZE);
	cv::imshow("Threshold lower image", lower_red_hue_range);
	cv::namedWindow("Threshold upper image", cv::WINDOW_AUTOSIZE);
	cv::imshow("Threshold upper image", upper_red_hue_range);
	cv::namedWindow("Combined threshold images", cv::WINDOW_AUTOSIZE);
	cv::imshow("Combined threshold images", red_hue_image);
	cv::namedWindow("Detected red circles on the input image", cv::WINDOW_AUTOSIZE);
	cv::imshow("Detected red circles on the input image", orig_image);

	cv::waitKey(0);*/


	return 0;
}

// ************************************************************

int thresh_ = 50, N = 7;
const char* wndname = "Square Detection Demo";

// helper function:
// finds a cosine of angle between vectors
// from pt0->pt1 and from pt0->pt2
double angle_(cv::Point pt1, cv::Point pt2, cv::Point pt0)
{
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1 * dx2 + dy1 * dy2) / sqrt((dx1 * dx1 + dy1 * dy1) * (dx2 * dx2 + dy2 * dy2) + 1e-10);
}

// returns sequence of squares detected on the image.
// the sequence is stored in the specified memory storage
void findSquares(const cv::Mat& image, vector<vector<cv::Point> >& squares)
{
	squares.clear();

	//s    Mat pyr, timg, gray0(image.size(), CV_8U), gray;

		// down-scale and upscale the image to filter out the noise
		//pyrDown(image, pyr, Size(image.cols/2, image.rows/2));
		//pyrUp(pyr, timg, image.size());


		// blur will enhance edge detection
	cv::Mat timg(image);
	//cv::medianBlur(image, timg, 9);
	//cv::Mat gray0(timg.size(), CV_8U), gray;
	cv::Mat gray0(image.size(), CV_8U), gray;

	vector<vector<cv::Point> > contours;

	// find squares in every color plane of the image
	for (int c = 0; c < 3; c++)
	{
		int ch[] = { c, 0 };
		mixChannels(&timg, 1, &gray0, 1, ch, 1);

		// try several threshold levels
		for (int l = 0; l < N; l++)
		{
			// hack: use Canny instead of zero threshold level.
			// Canny helps to catch squares with gradient shading
			if (l == 0)
			{
				// apply Canny. Take the upper threshold from slider
				// and set the lower to 0 (which forces edges merging)
				Canny(gray0, gray, 5, thresh_, 5);
				// dilate canny output to remove potential
				// holes between edge segments
				dilate(gray, gray, cv::Mat(), cv::Point(-1, -1));
			}
			else
			{
				// apply threshold if l!=0:
				//     tgray(x,y) = gray(x,y) < (l+1)*255/N ? 255 : 0
				gray = gray0 >= (l + 1) * 255 / N;
			}

			// find contours and store them all as a list
			findContours(gray, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

			vector<cv::Point> approx;

			// test each contour
			for (size_t i = 0; i < contours.size(); i++)
			{
				// approximate contour with accuracy proportional
				// to the contour perimeter
				approxPolyDP(cv::Mat(contours[i]), approx, cv::arcLength(cv::Mat(contours[i]), true) * 0.02, true);

				// square contours should have 4 vertices after approximation
				// relatively large area (to filter out noisy contours)
				// and be convex.
				// Note: absolute value of an area is used because
				// area may be positive or negative - in accordance with the
				// contour orientation
				if (approx.size() == 4 &&
					fabs(cv::contourArea(cv::Mat(approx))) > 1000 &&
					cv::isContourConvex(cv::Mat(approx)))
				{
					double maxCosine = 0;

					for (int j = 2; j < 5; j++)
					{
						// find the maximum cosine of the angle between joint edges
						double cosine = fabs(angle_(approx[j % 4], approx[j - 2], approx[j - 1]));
						maxCosine = MAX(maxCosine, cosine);
					}

					// if cosines of all angles are small
					// (all angles are ~90 degree) then write quandrange
					// vertices to resultant sequence
					if (maxCosine < 0.3)
						squares.push_back(approx);
				}
			}
		}
	}
}


// the function draws all the squares in the image
void drawSquares(cv::Mat& image, const vector<vector<cv::Point> >& squares)
{
	for (size_t i = 0; i < squares.size(); i++)
	{
		const cv::Point* p = &squares[i][0];

		int n = (int)squares[i].size();
		//dont detect the border
		if (p->x > 3 && p->y > 3)
			polylines(image, &p, &n, 1, true, cv::Scalar(0, 0, 255), 1, cv::LINE_AA);
	}

	

	//imshow(wndname, image);
}

void Bound_rect(int numcadr, cv::Mat& image)
{
	//help();
	//namedWindow(wndname, 1);
	vector<vector<cv::Point> > squares;

	//for (int i = 0; names[i] != 0; i++)
	{
		//Mat image = imread(names[i], 1);
		if (image.empty())
		{
			//cout << "Couldn't load " << names[i] << endl;
			//continue;
			return;
		}

		findSquares(image, squares);
		drawSquares(image, squares);
		//imwrite( "out", image );
		//int c = waitKey();
		//if ((char)c == 27) break;
	}
	
	InsertJsonSquare(numcadr, squares);

	return;
}

