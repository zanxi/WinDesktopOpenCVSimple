#pragma once

#include "stdafx.h"
#include "jsonlib.h"


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
	if (radius > 30 && radius < 40 && 40 < sqrt(r) && sqrt(r) < 100) return true;

	return false;
}

bool r_rectangle(cv::Rect rect, std::vector<std::vector<cv::Point> > squares)
{
	double r = 0;
	double S = (rect.br().x - rect.tl().x) * (rect.br().y - rect.tl().y);
	//for (auto& m : squares)
	// считаем среднее расстояние от вершин найденного контура до оболочки
	{
		r = (rect.br().x - squares[0][1].x) * (rect.br().x - squares[0][1].x) +
			(rect.br().y - squares[0][1].y) * (rect.br().y - squares[0][1].y) +
			(rect.tl().x - squares[0][0].x) * (rect.tl().x - squares[0][0].x) +
			(rect.tl().y - squares[0][0].y) * (rect.tl().y - squares[0][0].y);
	}
	//if (S>100&&140<sqrt(r)&& sqrt(r) < 200) return true;
	if (S > 100 && sqrt(r) < 50) return true;

	return false;
}


int Bound4(cv::Mat& img);

void Bound1(int numCadr, std::vector<std::vector<cv::Point> > squares, cv::Mat& src_mat)
{
	Bound4(src_mat);

	return;

	//std::vector<std::vector<cv::Point>> contours2;
	//std::vector<cv::Vec4i> hierarchy;
	//cv::findContours(src_mat, contours2, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

	//cv::Scalar red(0, 0, 255);
	//cv::drawContours(src_mat, contours2, -1, red, 2);
	////showImg(input, "Detected Contour");

	//return;

	//const char* file_name = "shape.png";

	//cv::Mat src_mat, gray_mat, canny_mat;
	cv::Mat gray_mat, canny_mat;
	cv::Mat contour_mat;
	//cv::Mat bounding_mat;

	const char* src_name = "Source";
	const char* contours_name = "Contours";
	const char* bounding_name = "Bounding";

	//1.Read image file & clone.
	//src_mat = cv::imread(file_name);
	contour_mat = src_mat.clone();
	//bounding_mat = src_mat.clone();

	//2. Convert to gray image and apply canny edge detection
	cvtColor(src_mat, gray_mat, cv::COLOR_RGB2GRAY);
	Canny(gray_mat, canny_mat, 30, 128, 3, false);

	//3. Find & process the contours
	//3.1 find contours on the edge image.
	std::vector<std::vector<cv::Point> > contours;
	findContours(canny_mat, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	//3.2 draw contours & property value on the source image.
	double area, length;
	drawContours(contour_mat, contours, -1, cv::Scalar(0), 2);	//draw contours on the image
	for (int i = 0; i < contours.size(); ++i)
	{
		cv::Point2f center;
		float radius;

		area = contourArea(contours[i]);
		length = arcLength(contours[i], true);
		minEnclosingCircle(contours[i], center, radius);

		//draw contour property value at the contour center.
		char buffer[64] = { 0 };
		//sprintf(buffer, "Area: %.2lf", area);
		//putText(contour_mat, buffer, center, cv::FONT_HERSHEY_SIMPLEX, .6, cv::Scalar(0), 1);

		//sprintf(buffer, "Length: %.2lf", length);
		//putText(contour_mat, buffer, cv::Point(center.x, center.y + 20), cv::FONT_HERSHEY_SIMPLEX, .6, cv::Scalar(0), 1);

	}

	//3.3 find bounding of each contour, and draw it on the source image.
	for (int i = 0; i < contours.size(); ++i)
	{
		cv::Point2f points[4];
		cv::Point2f center;
		float radius;
		cv::Rect rect;
		cv::RotatedRect rotate_rect;

		//compute the bounding rect, rotated bounding rect, minum enclosing circle.
		rect = boundingRect(contours[i]);
		rotate_rect = minAreaRect(contours[i]);
		minEnclosingCircle(contours[i], center, radius);

		rotate_rect.points(points);

		std::vector<std::vector<cv::Point> > polylines;
		polylines.resize(1);
		for (int j = 0; j < 4; ++j)
			polylines[0].push_back(points[j]);

		//draw them on the bounding image.
		if (r_rectangle(rect, squares))
		{
			InsertJsonRect(rect, numCadr);
			cv::rectangle(src_mat, rect, cv::Scalar(0, 0, 255), 2);
		}
		//cv::polylines(src_mat, polylines, true, cv::Scalar(0, 255, 0), 2);		
		if (r_center(radius, center.x, center.y, squares))
		{
			InsertJsonCircle(radius, center, numCadr);
			cv::circle(src_mat, center, radius, cv::Scalar(255, 0, 0), 2);
		}

	}
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
	//vector<vector<cv::Point> > contours;
	//vector<cv::Vec4i> hierarchy;

	//cv::Mat gray;// = image.clone();
	//cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);

	//cv::threshold(gray, gray, 1, 255, cv::THRESH_BINARY);
	////cv::xors(gray, cv::Scalar(255, 0, 0, 0), gray, NULL);
	//cv::dilate(gray, gray, NULL, cv::Point(0,0));

	//findContours(gray, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

	//cv::drawContours(image, contours, -1, CV_RGB(0, 0, 0), -1, cv::FILLED, 8);

	//for (size_t i = 0; i < contours.size(); i++)
	//{
	//	cv::Mat pointsf;
	//	cv::Mat(contours[i]).convertTo(pointsf, CV_32F);

	//	//cv::RotatedRect minRect = minAreaRect(cv::Mat(squares[i]));
	//	//cv::Point2f rect_points[4];
	//	//minRect.points(rect_points);
	//	//ellipse(image, minRect, cv::Scalar(0, 0, 255), 1, 8);

	//	cv::RotatedRect box = fitEllipse(pointsf);
	//	ellipse(image, box, cv::Scalar(0, 0, 255), 1, 8);
	//}

	//cv::Mat gray, storage;
	//cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
	////cv::cvsm(gray, gray, cv::GaussianBlur, 7, 7);

	////IplImage* cc_img = cv::CreateImage(cv::Size(gray), gray->depth, 3);
	////cv::SetZero(cc_img);
	//cv::Scalar(ext_color);

	//cv::Canny(gray, gray, 10, 30, 3);

	////cv::memStorage* storage = cv::storag createMemStorage(0);
	//cv::HoughCircles(gray, storage, cv::HOUGH_GRADIENT, 1, 40 / 6, 100, 50);
	//cv::cvtColor(gray, src, cv::COLOR_GRAY2BGR);
	//for (size_t i = 0; i < storage->total; i++)
	//{
	//	// round the floats to an int
	//	float* p = (float*)cv::getElemSize(circles, i);
	//	cv::Point center(cvRound(p[0]), cvRound(p[1]));
	//	int radius = cvRound(p[2]);

	//	// draw the circle center
	//	//cvCircle(cc_img, center, 3, CV_RGB(0,255,0), -1, 8, 0 );

	//	// draw the circle outline
	//	cv::circle(cc_img, center, radius + 1, CV_RGB(0, 0, 255), 2, 8, 0);

	//	//printf("x: %d y: %d r: %d\n", center.x, center.y, radius);
	//}

	//CvMemStorage* mem;
	//mem = cvCreateMemStorage(0);
	//CvSeq* contours = 0;
	//cvCvtColor(cc_img, gray, CV_BGR2GRAY);
	//// Use either this:
	//int n = cv::findContours(gray, mem, &contours, sizeof(cv::cvContour), cv::RETR_CCOMP, cv::CHAIN_APPROX_NONE, cv::Point(0, 0));
	//// Or this:
	////int n = cvFindContours(gray, mem, &contours, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));

	//for (; contours != 0; contours = contours->h_next)
	//{
	//	ext_color = CV_RGB(rand() & 255, rand() & 255, rand() & 255); //randomly coloring different contours
	//	cv::drawContours(cc_img, contours, ext_color, CV_RGB(0, 0, 0), -1, CV_FILLED, 8, cvPoint(0, 0));
	//}

	//***************************************
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
	if (label != "CIR")return;

	int fontface = cv::FONT_HERSHEY_SIMPLEX;
	double scale = 0.4;
	int thickness = 1;
	int baseline = 0;

	cv::Size text = cv::getTextSize(label, fontface, scale, thickness, &baseline);
	cv::Rect r = cv::boundingRect(contour);

	cv::Point pt(r.x + ((r.width - text.width) / 2), r.y + ((r.height + text.height) / 2));
	cv::rectangle(im, pt + cv::Point(0, baseline), pt + cv::Point(text.width, -text.height), CV_RGB(255, 255, 255), cv::FILLED);
	cv::putText(im, label, pt, fontface, scale, CV_RGB(0, 0, 0), thickness, 8);
}


void Bound2(cv::Mat& src)
{
	return;
	//cv::Mat src = cv::imread("polygon.png");
	//cv::Mat src = cv::imread("assets/basic-shapes-2.png");

	// Convert to grayscale

	cv::Mat contour_mat = src.clone();

	cv::Mat gray;
	cv::cvtColor(contour_mat, gray, cv::COLOR_BGR2GRAY);

	// Use Canny instead of threshold to catch squares with gradient shading
	cv::Mat bw;
	cv::Canny(gray, bw, 0, 50, 5);

	// Find contours
	std::vector<std::vector<cv::Point> > contours;
	cv::findContours(bw.clone(), contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	std::vector<cv::Point> approx;
	cv::Mat dst = src.clone();

	for (int i = 0; i < contours.size(); i++)
	{
		// Approximate contour with accuracy proportional
		// to the contour perimeter
		cv::approxPolyDP(cv::Mat(contours[i]), approx, cv::arcLength(cv::Mat(contours[i]), true) * 0.02, true);

		// Skip small or non-convex objects 
		if (std::fabs(cv::contourArea(contours[i])) < 100 || !cv::isContourConvex(approx))
			continue;

		if (approx.size() == 3)
		{
			setLabel(src, "TRI", contours[i]);    // Triangles
		}
		else if (approx.size() >= 4 && approx.size() <= 6)
		{
			// Number of vertices of polygonal curve
			int vtc = approx.size();

			// Get the cosines of all corners
			std::vector<double> cos;
			for (int j = 2; j < vtc + 1; j++)
				cos.push_back(angle(approx[j % vtc], approx[j - 2], approx[j - 1]));

			// Sort ascending the cosine values
			std::sort(cos.begin(), cos.end());

			// Get the lowest and the highest cosine
			double mincos = cos.front();
			double maxcos = cos.back();

			// Use the degrees obtained above and the number of vertices
			// to determine the shape of the contour
			if (vtc == 4 && mincos >= -0.1 && maxcos <= 0.3)
				setLabel(src, "RECT", contours[i]);
			else if (vtc == 5 && mincos >= -0.34 && maxcos <= -0.27)
				setLabel(src, "PENTA", contours[i]);
			else if (vtc == 6 && mincos >= -0.55 && maxcos <= -0.45)
				setLabel(src, "HEXA", contours[i]);
		}
		else
		{
			// Detect and label circles
			double area = cv::contourArea(contours[i]);
			cv::Rect r = cv::boundingRect(contours[i]);
			int radius = r.width / 2;

			if (std::abs(1 - ((double)r.width / r.height)) <= 0.2 &&
				std::abs(1 - (area / (CV_PI * std::pow(radius, 2)))) <= 0.2)
			{
				cv::Scalar red(0, 0, 255);
				cv::drawContours(src, contours[i], radius, red, 2);
				setLabel(src, "CIR", contours[i]);
			}
		}
	}
}



// **********************************************
cv::Mat img_gray, input_img;
int thresh = 100;
const int MAX_THRESH = 255;


void draw_and_fill_contours(std::vector<std::vector<cv::Point>>& contours,
	std::vector<std::vector<cv::Point>>& hull,
	std::vector<cv::Vec4i>& hierarchy)
{

	cv::Mat contours_result = input_img.clone();
	cv::Mat fill_contours_result = cv::Mat::zeros(img_gray.size(), CV_8UC3);

	for (unsigned int i = 0, n = contours.size(); i < n; ++i)
	{
		cv::Scalar color = cv::Scalar(0, 0, 255);
		cv::drawContours(contours_result, contours, i, color, 4, 8, hierarchy, 0, cv::Point());
	}

	cv::fillPoly(fill_contours_result, hull, cv::Scalar(255, 255, 255));

	//cv::imshow("Contours Result", contours_result);
	//cv::imshow("Fill Contours Result", fill_contours_result);

}




void find_contours(int, void*) {
	cv::Mat canny_output;
	cv::Canny(img_gray, canny_output, thresh, thresh * 2);
	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(canny_output, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

	std::vector<std::vector<cv::Point>> hull(contours.size());
	for (unsigned int i = 0, n = contours.size(); i < n; ++i) {
		cv::convexHull(cv::Mat(contours[i]), hull[i], false);
	}

	draw_and_fill_contours(contours, hull, hierarchy);
}

void Bound3(cv::Mat& input_img)
{
	cv::cvtColor(input_img, img_gray, cv::COLOR_BGR2GRAY);
	cv::blur(img_gray, img_gray, cv::Size(3, 3));
	const std::string source_window("Source");
	//cv::namedWindow(source_window.c_str());
	//cv::imshow(source_window.c_str(), input_img);

	//cv::createTrackbar("Thresh: ", source_window, &thresh, MAX_THRESH, find_contours);
	find_contours(0, 0);
}


// **********************************************
int Bound4(cv::Mat& img)
{
	if (img.empty())
	{
		cout << "Could not open image..." << endl;
		return -1;
	}

	cvtColor(img, img, cv::COLOR_BGR2GRAY);

	int dilation_type = 0;
	int dilation_elem = 0;

	if (dilation_elem == 0) { dilation_type = cv::MORPH_RECT; }
	else if (dilation_elem == 1) { dilation_type = cv::MORPH_CROSS; }
	else if (dilation_elem == 2) { dilation_type = cv::MORPH_ELLIPSE; }

	int size = 1;

	cv::Mat element = cv::getStructuringElement(dilation_type, cv::Size(2 * size + 1, 2 * size + 1), cv::Point(size, size));
	morphologyEx(img, img, cv::MORPH_OPEN, element);

	vector<cv::Point2f> points;
	for (int x = 0; x < img.cols; x++)
	{
		for (int y = 0; y < img.rows; y++)
		{
			if (img.at<uchar>(y, x) > 0)
			{
				points.push_back(cv::Point2f(x, y));
			}
		}
	}

	//// Least Square Algorithm 

	float xn = 0, xsum = 0;
	float yn = 0, ysum = 0;
	float n = points.size();

	for (int i = 0; i < n; i++)
	{
		xsum = xsum + points[i].x;
		ysum = ysum + points[i].y;
	}

	xn = xsum / n;
	yn = ysum / n;

	float ui = 0;
	float vi = 0;
	float suu = 0, suuu = 0;
	float svv = 0, svvv = 0;
	float suv = 0;
	float suvv = 0, svuu = 0;

	for (int i = 0; i < n; i++)
	{
		ui = points[i].x - xn;
		vi = points[i].y - yn;

		suu = suu + (ui * ui);
		suuu = suuu + (ui * ui * ui);

		svv = svv + (vi * vi);
		svvv = svvv + (vi * vi * vi);

		suv = suv + (ui * vi);

		suvv = suvv + (ui * vi * vi);
		svuu = svuu + (vi * ui * ui);
	}

	cv::Mat A = (cv::Mat_<float>(2, 2) <<
		suu, suv,
		suv, svv);

	cv::Mat B = (cv::Mat_<float>(2, 1) <<
		0.5 * (suuu + suvv),
		0.5 * (svvv + svuu));

	cv::Mat abc;
	cv::solve(A, B, abc);

	float u = abc.at<float>(0);
	float v = abc.at<float>(1);

	float x = u + xn;
	float y = v + yn;

	float alpha = u * u + v * v + ((suu + svv) / n);
	float r = sqrt(alpha);

	////

	cvtColor(img, img, cv::COLOR_GRAY2BGR);

	// Draw circle
	circle(img, cv::Point(x, y), r, cv::Scalar(255, 0, 0), 1, 8, 0);
	//imshow("window", img);
	//waitKey(0);
	return 0;
}

// **********************************************


std::vector<cv::Rect> getRedObjects(cv::Mat image);
vector<cv::Mat> detectAndDisplaySpeedLimit(cv::Mat frame);
vector<cv::Mat> detectAndDisplayNoParking(cv::Mat frame);
vector<cv::Mat> detectAndDisplayWarning(cv::Mat frame);
void trainDigitClassifier(cv::Mat& src);
string getDigits(cv::Mat image);
vector<cv::Mat> loadAllImage();
int getSpeedLimit(string speed);

//path of the haar cascade files
string no_parking_signs_cascade = "/Users/giuliopettenuzzo/Desktop/cascade_classifiers/no_parking_cascade.xml";
string speed_signs_cascade = "/Users/giuliopettenuzzo/Desktop/cascade_classifiers/speed_limit_cascade.xml";
string warning_signs_cascade = "/Users/giuliopettenuzzo/Desktop/cascade_classifiers/warning_cascade.xml";

cv::CascadeClassifier speed_limit_cascade;
cv::CascadeClassifier no_parking_cascade;
cv::CascadeClassifier warning_cascade;

int CV_HAAR_SCALE_IMAGE = 5;
int SCALE_IMAGE = 5;

int Bound5(cv::Mat& src)
{
	//train the classifier for digit recognition, this require a manually train, read the report for more details
	trainDigitClassifier(src);

	cv::Mat sceneImage;
	vector<cv::Mat> allImages = loadAllImage();

	for (int i = 0; i <= allImages.size(); i++) {
		sceneImage = allImages[i];

		//load the haar cascade files
		if (!speed_limit_cascade.load(speed_signs_cascade)) { printf("--(!)Error loading\n"); return -1; };
		if (!no_parking_cascade.load(no_parking_signs_cascade)) { printf("--(!)Error loading\n"); return -1; };
		if (!warning_cascade.load(warning_signs_cascade)) { printf("--(!)Error loading\n"); return -1; };

		cv::Mat scene = sceneImage.clone();

		//detect the red objects
		std::vector<cv::Rect> allObj = getRedObjects(scene);

		//use the three cascade classifier for each object detected by the getRedObjects() method
		for (int j = 0; j < allObj.size(); j++) {
			cv::Mat img = sceneImage(cv::Rect(allObj[j]));
			vector<cv::Mat> warningVec = detectAndDisplayWarning(img);
			if (warningVec.size() > 0) {
				cv::Rect box = allObj[j];
			}
			vector<cv::Mat> noParkVec = detectAndDisplayNoParking(img);
			if (noParkVec.size() > 0) {
				cv::Rect box = allObj[j];
			}
			vector<cv::Mat> speedLitmitVec = detectAndDisplaySpeedLimit(img);
			if (speedLitmitVec.size() > 0) {
				cv::Rect box = allObj[j];
				for (int i = 0; i < speedLitmitVec.size(); i++) {
					//get speed limit and skatch it in the image
					int digit = getSpeedLimit(getDigits(speedLitmitVec[i]));
					if (digit > 0) {
						cv::Point point = box.tl();
						point.y = point.y + 30;
						cv::putText(sceneImage,
							"SPEED LIMIT " + to_string(digit),
							point,
							cv::FONT_HERSHEY_COMPLEX_SMALL,
							0.7,
							cv::Scalar(0, 255, 0),
							1,
							cv::CV__CAP_PROP_LATEST);
					}
				}
			}
		}
		//imshow("currentobj", sceneImage);
		//waitKey(0);
	}
}

/*
 *  detect the red object in the image given in the param,
 *  return a vector containing all the Rect of the red objects
 */
std::vector<cv::Rect> getRedObjects(cv::Mat image)
{
	cv::Mat3b res = image.clone();
	std::vector<cv::Rect> result;

	cvtColor(image, image, cv::COLOR_BGR2HSV);

	cv::Mat1b mask1, mask2;
	//ranges of red color
	inRange(image, cv::Scalar(0, 70, 50), cv::Scalar(10, 255, 255), mask1);
	inRange(image, cv::Scalar(170, 70, 50), cv::Scalar(180, 255, 255), mask2);

	cv::Mat1b mask = mask1 | mask2;
	cv::Mat nonZeroCoordinates;
	vector<cv::Point> pts;

	cv::findNonZero(mask, pts);
	for (int i = 0; i < nonZeroCoordinates.total(); i++) {
		std::cout << "Zero#" << i << ": " << nonZeroCoordinates.at<cv::Point>(i).x << ", " << nonZeroCoordinates.at<cv::Point>(i).y << endl;
	}

	int th_distance = 2; // radius tolerance

	 // Apply partition
	 // All pixels within the radius tolerance distance will belong to the same class (same label)
	vector<int> labels;

	// With lambda function (require C++11)
	int th2 = th_distance * th_distance;
	int n_labels = partition(pts, labels, [th2](const cv::Point& lhs, const cv::Point& rhs) {
		return ((lhs.x - rhs.x) * (lhs.x - rhs.x) + (lhs.y - rhs.y) * (lhs.y - rhs.y)) < th2;
		});

	// You can save all points in the same class in a vector (one for each class), just like findContours
	vector<vector<cv::Point>> contours(n_labels);
	for (int i = 0; i < pts.size(); ++i) {
		contours[labels[i]].push_back(pts[i]);
	}

	// Get bounding boxes
	vector<cv::Rect> boxes;
	for (int i = 0; i < contours.size(); ++i)
	{
		cv::Rect box = boundingRect(contours[i]);
		if (contours[i].size() > 500) {//prima era 1000
			boxes.push_back(box);

			cv::Rect enlarged_box = box + cv::Size(100, 100);
			enlarged_box -= cv::Point(30, 30);

			if (enlarged_box.x < 0) {
				enlarged_box.x = 0;
			}
			if (enlarged_box.y < 0) {
				enlarged_box.y = 0;
			}
			if (enlarged_box.height + enlarged_box.y > res.rows) {
				enlarged_box.height = res.rows - enlarged_box.y;
			}
			if (enlarged_box.width + enlarged_box.x > res.cols) {
				enlarged_box.width = res.cols - enlarged_box.x;
			}

			cv::Mat img = res(cv::Rect(enlarged_box));
			result.push_back(enlarged_box);
		}
	}
	cv::Rect largest_box = *max_element(boxes.begin(), boxes.end(), [](const cv::Rect& lhs, const cv::Rect& rhs) {
		return lhs.area() < rhs.area();
		});

	//draw the rects in case you want to see them
	for (int j = 0; j <= boxes.size(); j++) {
		if (boxes[j].area() > largest_box.area() / 3) {
			rectangle(res, boxes[j], cv::Scalar(0, 0, 255));

			cv::Rect enlarged_box = boxes[j] + cv::Size(20, 20);
			enlarged_box -= cv::Point(10, 10);

			rectangle(res, enlarged_box, cv::Scalar(0, 255, 0));
		}
	}

	rectangle(res, largest_box, cv::Scalar(0, 0, 255));

	cv::Rect enlarged_box = largest_box + cv::Size(20, 20);
	enlarged_box -= cv::Point(10, 10);

	rectangle(res, enlarged_box, cv::Scalar(0, 255, 0));

	return result;
}

/*
 *  code for detect the speed limit sign , it draws a circle around the speed limit signs
 */
vector<cv::Mat> detectAndDisplaySpeedLimit(cv::Mat frame)
{
	std::vector<cv::Rect> signs;
	vector<cv::Mat> result;
	cv::Mat frame_gray;

	cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);
	//normalizes the brightness and increases the contrast of the image
	equalizeHist(frame_gray, frame_gray);

	//-- Detect signs
	speed_limit_cascade.detectMultiScale(frame_gray, signs, 1.1, 3, 0 | CV_HAAR_SCALE_IMAGE, cv::Size(30, 30));
	//std::cout << speed_limit_cascade.getFeatureType();

	for (size_t i = 0; i < signs.size(); i++)
	{
		cv::Point center(signs[i].x + signs[i].width * 0.5, signs[i].y + signs[i].height * 0.5);
		ellipse(frame, center, cv::Size(signs[i].width * 0.5, signs[i].height * 0.5), 0, 0, 360, cv::Scalar(255, 0, 255), 4, 8, 0);


		cv::Mat resultImage = frame(cv::Rect(center.x - signs[i].width * 0.5, center.y - signs[i].height * 0.5, signs[i].width, signs[i].height));
		result.push_back(resultImage);
	}
	return result;
}

/*
 *  code for detect the warning sign , it draws a circle around the warning signs
 */
vector<cv::Mat> detectAndDisplayWarning(cv::Mat frame)
{
	std::vector<cv::Rect> signs;
	vector<cv::Mat> result;
	cv::Mat frame_gray;

	cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);

	//-- Detect signs
	warning_cascade.detectMultiScale(frame_gray, signs, 1.1, 3, 0 | SCALE_IMAGE, cv::Size(30, 30));
	//cout << warning_cascade.getFeatureType();
	cv::Rect previus;


	for (size_t i = 0; i < signs.size(); i++)
	{
		cv::Point center(signs[i].x + signs[i].width * 0.5, signs[i].y + signs[i].height * 0.5);
		cv::Rect newRect = cv::Rect(center.x - signs[i].width * 0.5, center.y - signs[i].height * 0.5, signs[i].width, signs[i].height);
		if ((previus & newRect).area() > 0) {
			previus = newRect;
		}
		else {
			ellipse(frame, center, cv::Size(signs[i].width * 0.5, signs[i].height * 0.5), 0, 0, 360, cv::Scalar(0, 0, 255), 4, 8, 0);
			cv::Mat resultImage = frame(newRect);
			result.push_back(resultImage);
			previus = newRect;
		}
	}
	return result;
}

/*
 *  code for detect the no parking sign , it draws a circle around the no parking signs
 */
vector<cv::Mat> detectAndDisplayNoParking(cv::Mat frame)
{
	std::vector<cv::Rect> signs;
	vector<cv::Mat> result;
	cv::Mat frame_gray;

	cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);

	//-- Detect signs
	no_parking_cascade.detectMultiScale(frame_gray, signs, 1.1, 3, 0 | CV_HAAR_SCALE_IMAGE, cv::Size(30, 30));
	//cout << no_parking_cascade.getFeatureType();
	cv::Rect previus;

	for (size_t i = 0; i < signs.size(); i++)
	{
		cv::Point center(signs[i].x + signs[i].width * 0.5, signs[i].y + signs[i].height * 0.5);
		cv::Rect newRect = cv::Rect(center.x - signs[i].width * 0.5, center.y - signs[i].height * 0.5, signs[i].width, signs[i].height);
		if ((previus & newRect).area() > 0) {
			previus = newRect;
		}
		else {
			ellipse(frame, center, cv::Size(signs[i].width * 0.5, signs[i].height * 0.5), 0, 0, 360, cv::Scalar(255, 0, 0), 4, 8, 0);
			cv::Mat resultImage = frame(newRect);
			result.push_back(resultImage);
			previus = newRect;
		}
	}
	return result;
}

/*
 *  train the classifier for digit recognition, this could be done only one time, this method save the result in a file and
 *  it can be used in the next executions
 *  in order to train user must enter manually the corrisponding digit that the program shows, press space if the red box is just a point (false positive)
 */
void trainDigitClassifier(cv::Mat& src) {
	cv::Mat thr, gray, con;
	//cv::Mat src = imread("/Users/giuliopettenuzzo/Desktop/all_numbers.png", 1);
	cvtColor(src, gray, cv::COLOR_BGR2GRAY);
	threshold(gray, thr, 125, 255, cv::THRESH_BINARY_INV); //Threshold to find contour
	//imshow("ci", thr);
	//waitKey(0);
	thr.copyTo(con);

	// Create sample and label data
	vector< vector <cv::Point> > contours; // Vector for storing contour
	vector< cv::Vec4i > hierarchy;
	cv::Mat sample;
	cv::Mat response_array;
	findContours(con, contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE); //Find contour

	for (int i = 0; i < contours.size(); i = hierarchy[i][0]) // iterate through first hierarchy level contours
	{
		cv::Rect r = boundingRect(contours[i]); //Find bounding rect for each contour
		rectangle(src, cv::Point(r.x, r.y), cv::Point(r.x + r.width, r.y + r.height), cv::Scalar(0, 0, 255), 2, 8, 0);
		cv::Mat ROI = thr(r); //Crop the image
		cv::Mat tmp1, tmp2;
		cv::resize(ROI, tmp1, cv::Size(10, 10), 0, 0, cv::INTER_LINEAR); //resize to 10X10
		tmp1.convertTo(tmp2, CV_32FC1); //convert to float

		//imshow("src", src);

		int c = cv::waitKey(0); // Read corresponding label for contour from keyoard
		c -= 0x30;     // Convert ascii to intiger value
		response_array.push_back(c); // Store label to a mat
		rectangle(src, cv::Point(r.x, r.y), cv::Point(r.x + r.width, r.y + r.height), cv::Scalar(0, 255, 0), 2, 8, 0);
		sample.push_back(tmp2.reshape(1, 1)); // Store  sample data
	}

	// Store the data to file
	cv::Mat response, tmp;
	tmp = response_array.reshape(1, 1); //make continuous
	tmp.convertTo(response, CV_32FC1); // Convert  to float

	cv::FileStorage Data("TrainingData.yml", cv::FileStorage::WRITE); // Store the sample data in a file
	//Data << "data" << sample;
	Data.release();

	cv::FileStorage Label("LabelData.yml", cv::FileStorage::WRITE); // Store the label data in a file
	//cv::Label << "label" << response;
	Label.release();
	//cout << "Training and Label data created successfully....!! " << endl;

	//imshow("src", src);
	//waitKey(0);


}

/*
 *  get digit from the image given in param, using the classifier trained before
 */
string getDigits(cv::Mat image)
{
	cv::Mat thr1, gray1, con1;
	cv::Mat src1 = image.clone();
	cv::cvtColor(src1, gray1, cv::COLOR_BGR2GRAY);
	threshold(gray1, thr1, 125, 255, cv::THRESH_BINARY_INV); // Threshold to create input
	thr1.copyTo(con1);


	// Read stored sample and label for training
	cv::Mat sample1;
	cv::Mat response1, tmp1;
	cv::FileStorage Data1("TrainingData.yml", cv::FileStorage::READ); // Read traing data to a Mat
	Data1["data"] >> sample1;
	Data1.release();

	cv::FileStorage Label1("LabelData.yml", cv::FileStorage::READ); // Read label data to a Mat
	Label1["label"] >> response1;
	Label1.release();


	cv::Ptr<cv::ml::KNearest>  knn(cv::ml::KNearest::create());

	knn->train(sample1, cv::ml::ROW_SAMPLE, response1); // Train with sample and responses
	cout << "Training compleated.....!!" << endl;

	vector< vector <cv::Point> > contours1; // Vector for storing contour
	vector< cv::Vec4i > hierarchy1;

	//Create input sample by contour finding and cropping
	findContours(con1, contours1, hierarchy1, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE);
	cv::Mat dst1(src1.rows, src1.cols, CV_8UC3, cv::Scalar::all(0));
	string result;

	for (int i = 0; i < contours1.size(); i = hierarchy1[i][0]) // iterate through each contour for first hierarchy level .
	{
		cv::Rect r = boundingRect(contours1[i]);
		cv::Mat ROI = thr1(r);
		cv::Mat tmp1, tmp2;
		resize(ROI, tmp1, cv::Size(10, 10), 0, 0, cv::INTER_LINEAR);
		tmp1.convertTo(tmp2, CV_32FC1);
		cv::Mat bestLabels;
		float p = knn->findNearest(tmp2.reshape(1, 1), 4, bestLabels);
		char name[4];
		sprintf(name, "%d", (int)p);
		cout << "num = " << (int)p;
		result = result + to_string((int)p);

		putText(dst1, name, cv::Point(r.x, r.y + r.height), 0, 1, cv::Scalar(0, 255, 0), 2, 8);
	}

	imwrite("dest.jpg", dst1);
	return  result;
}
/*
 *  from the digits detected, it returns a speed limit if it is detected correctly, -1 otherwise
 */
int getSpeedLimit(string numbers) {
	if ((numbers.find("30") != std::string::npos) || (numbers.find("03") != std::string::npos)) {
		return 30;
	}
	if ((numbers.find("50") != std::string::npos) || (numbers.find("05") != std::string::npos)) {
		return 50;
	}
	if ((numbers.find("80") != std::string::npos) || (numbers.find("08") != std::string::npos)) {
		return 80;
	}
	if ((numbers.find("70") != std::string::npos) || (numbers.find("07") != std::string::npos)) {
		return 70;
	}
	if ((numbers.find("90") != std::string::npos) || (numbers.find("09") != std::string::npos)) {
		return 90;
	}
	if ((numbers.find("100") != std::string::npos) || (numbers.find("001") != std::string::npos)) {
		return 100;
	}
	if ((numbers.find("130") != std::string::npos) || (numbers.find("031") != std::string::npos)) {
		return 130;
	}
	return -1;
}

/*
 *  load all the image in the file with the path hard coded below
 */
vector<cv::Mat> loadAllImage() {
	vector<cv::String> fn;
	cv::glob("/Users/giuliopettenuzzo/Desktop/T1/dataset/*.jpg", fn, false);

	vector<cv::Mat> images;
	return images;
	size_t count = fn.size(); //number of png files in images folder
	for (size_t i = 0; i < count; i++)
		images.push_back(cv::imread(fn[i]));
	return images;
}


